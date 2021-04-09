// Copyright (C) 2020 Alejandro G�ereca Valdivia / Jonathan Dean Damiani

#include "BGGameInstance.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSessionSettings.h"
#include "Kismet/GameplayStatics.h"
#include "UI/MainMenuBase.h"
#include "UI/LobbyMenuBase.h"
#include "Components/Widget.h"
#include "Characters/Player/PlayerBase.h"
#include "OnlineSubsystem.h"
#include "GameFramework/PlayerState.h"

void UBGGameInstance::Init()
{
	Super::Init();

	DesiredServerName = "DefaultName";

	IOnlineSubsystem* SubSystem = IOnlineSubsystem::Get();

	if (SubSystem == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Fail to find subsystem"));
		return;
	}

	SessionInterface = SubSystem->GetSessionInterface();

	if (SessionInterface.IsValid())
	{
		SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UBGGameInstance::OnCreateSessionComplete);
		SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UBGGameInstance::OnFindSessionsComplete);
		SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UBGGameInstance::OnJoinSessionsComplete);
		SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UBGGameInstance::OnDestroySessionComplete);
	}
}

void UBGGameInstance::LoadMenu()
{
	if (MainMenuUserWidget == nullptr) 
	{ 
		return ;
	}

	MainMenu = CreateWidget<UMainMenuBase>(this, MainMenuUserWidget);

	MainMenu->AddToViewport();
	MainMenu->SetInterface(this);

	SetPlayerUIInputMode(MainMenu);

	//////////////////////////////////////////////////////////////////////////
	// Comment for testing in Lan

	IOnlineSubsystem* SubSystem = IOnlineSubsystem::Get("Steam");

	if (SubSystem == nullptr)
	{
		MainMenu->DisableMultiplayer();
	}
}

void UBGGameInstance::Host()
{
	FOnlineSessionSettings SessionSettings;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bIsDedicated = false;
	SessionSettings.bIsLANMatch = CheckIfIsLan();
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bUsesPresence = true;
	SessionSettings.NumPublicConnections = 4;

	APlayerState* PS = GetPrimaryPlayerController()->PlayerState;

	if (PS != nullptr)
	{
		SetServerName(PS->GetPlayerName());
	}

	SessionSettings.Set(FName("SERVER_NAME_KEY"), DesiredServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	ServerInfo.ServerName = DesiredServerName;
	ServerInfo.CurrentPlayers = 0;
	ServerInfo.MaxPlayers = 4;
	ServerInfo.ServerArrayIndex = 0;

	SessionInterface->CreateSession(0, BGSessionName, SessionSettings);
}

void UBGGameInstance::EndSession()
{
	SessionInterface->DestroySession(BGSessionName);
}

void UBGGameInstance::JoinServer(int32 ServArrayIndex)
{
	FOnlineSessionSearchResult Result = SessionSearch->SearchResults[ServArrayIndex];

	FString MyServerName = "";

	Result.Session.SessionSettings.Get(FName("SERVER_NAME_KEY"), MyServerName);
	
	BGSessionName = FName(MyServerName);

	if (Result.IsValid())
	{
		SessionInterface->JoinSession(0, BGSessionName, Result);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to join"));
	}
}

void UBGGameInstance::SetServerName(FString CurrentServerName)
{
	DesiredServerName = CurrentServerName;
	BGSessionName = FName(CurrentServerName);
}

void UBGGameInstance::OpenSessionListMenu()
{
	SearchingServersDelegate.Broadcast(true);

	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->bIsLanQuery = CheckIfIsLan();
	SessionSearch->MaxSearchResults = 100;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}

bool UBGGameInstance::CheckIfSteamIsOpen()
{
	IOnlineSubsystem* SubSystem = IOnlineSubsystem::Get("Steam");

	if (SubSystem == nullptr)
	{
		return false;
	}

	return true;
}

void UBGGameInstance::SetPlayerGameInputMode()
{
	APlayerController* PC = GetFirstLocalPlayerController();

	if (PC == nullptr) 
	{
		return;
	}

	FInputModeGameOnly InputMode;

	PC->SetInputMode(InputMode);

	PC->bShowMouseCursor = false;
}

void UBGGameInstance::SetPlayerUIInputMode(UWidget* UserWidget)
{
	APlayerController* PC = GetFirstLocalPlayerController();

	if (PC == nullptr) 
	{
		return;
	}

	FInputModeGameAndUI InputModeUI;

	InputModeUI.SetWidgetToFocus(UserWidget->TakeWidget());
	InputModeUI.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	PC->SetInputMode(InputModeUI);

	PC->bShowMouseCursor = true;
}

void UBGGameInstance::OnCreateSessionComplete(FName SessionName, bool Success)
{
	if (!Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("UNSUCESS"));
		return;
	}

	GetWorld()->ServerTravel("/Game/Maps/Levels/Level_Lobby?listen");
}

void UBGGameInstance::OnDestroySessionComplete(FName SessionName, bool Success)
{
	// TODO Handle Session Destroy
	UE_LOG(LogTemp, Warning, TEXT("On Destroy"));
}

void UBGGameInstance::OnFindSessionsComplete(bool Success)
{
	// It will not be success if there are more than one session with the same name already created
	if (!Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("UNSUCESS"));
		return;
	}

	TArray<FOnlineSessionSearchResult> SearchResults = SessionSearch->SearchResults;

	SearchingServersDelegate.Broadcast(false);

	int32 MyArrayIndex = 0;

	if (SearchResults.Num() > 0)
	{
		for (FOnlineSessionSearchResult Result : SearchResults)
		{
			if (!Result.IsValid())
			{
				continue;
			}

			FString MyServerName = "";

			Result.Session.SessionSettings.Get(FName("SERVER_NAME_KEY"), MyServerName);

			FAllServeInfo AllServeInfo;
			AllServeInfo.ServerName = MyServerName;
			AllServeInfo.MaxPlayers = Result.Session.NumOpenPublicConnections;
			AllServeInfo.CurrentPlayers = AllServeInfo.MaxPlayers - Result.Session.NumOpenPublicConnections;
			AllServeInfo.ServerArrayIndex = MyArrayIndex;
			AllServeInfoDelegate.Broadcast(AllServeInfo);

			MyArrayIndex++;
		}
	}
	
}

void UBGGameInstance::OnJoinSessionsComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	UE_LOG(LogTemp, Warning, TEXT("OnJoinSessionsComplete, SessionName: %s"), *SessionName.ToString());

	if (APlayerController* PController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		FString JoinAddress = "";
		SessionInterface->GetResolvedConnectString(SessionName, JoinAddress);

		if (JoinAddress != "")
		{
			UE_LOG(LogTemp, Warning, TEXT("Joining server"));
			UE_LOG(LogTemp, Warning, TEXT("%s"), *JoinAddress);
			PController->ClientTravel(JoinAddress, ETravelType::TRAVEL_Absolute);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No Player Controller"));
	}
}

bool UBGGameInstance::CheckIfIsLan() const
{
	if (IOnlineSubsystem::Get()->GetSubsystemName() == "Steam")
	{
		return false;
	}
	else
	{
		return true;
	}
}

UMainMenuBase* UBGGameInstance::GetMainMenu()
{
	return MainMenu;
}

void UBGGameInstance::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UBGGameInstance, GameSessionId);
}
