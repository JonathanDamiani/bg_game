// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani

#include "Characters/Player/PlayerControllerBase.h"
#include "AbilitySystemComponent.h"
#include "UI/PlayerHUDBase.h"
#include "UI/OptionsMenuBase.h"
#include "UI/EndGameScreenBase.h"
#include "Characters/Player/PlayerBase.h"
#include "Characters/Player/PlayerStateBase.h"
#include "BGGameModeBase.h"
#include "BGGameInstance.h"
#include "GameFramework/PlayerStart.h"
#include "EngineUtils.h"
#include "Characters/Player/BGPlayerCameraManager.h"
#include "BGGameState.h"
#include "Kismet/GameplayStatics.h"

void APlayerControllerBase::CreatePlayerHUD()
{
	APlayerStateBase* PS = GetPlayerState<APlayerStateBase>();
	if (!PS)
	{
		return;
	}

	if (PlayerHUD != nullptr)
	{
		PlayerHUD->SetAllIconsWidgets();
		return;
	}

	if (PlayerHUDClass == nullptr)
	{
		return;
	}

	if (!IsLocalPlayerController())
	{
		return;
	}

	PlayerHUD = CreateWidget<UPlayerHUDBase>(this, PlayerHUDClass);

	if (PlayerHUD)
	{
		PlayerHUD->AddToViewport();
	}

	PS->SetPlayerInfo();
	PlayerHUD->SetCurrentHealth(PS->GetHealth());
	PlayerHUD->SetMaxHealth(PS->GetMaxHealth());
	PlayerHUD->SetHealthPercentage(PS->GetHealth() / PS->GetMaxHealth());

	PlayerHUD->SetAllIconsWidgets();
	SetPlayersInfoEvent();
}

void APlayerControllerBase::RemovePlayerHUD()
{
	if (PlayerHUD == nullptr)
	{
		return;
	}

	if (!IsLocalPlayerController())
	{
		return;
	}

	PlayerHUD->RemoveFromParent();
	PlayerHUD = nullptr;
}

void APlayerControllerBase::CreateOptionMenu()
{
	if (PlayerOptions != nullptr)
	{
		return;
	}

	if (PlayerOptionsClass == nullptr)
	{
		return;
	}

	if (!IsLocalPlayerController())
	{
		return;
	}

	PlayerOptions = CreateWidget<UOptionsMenuBase>(this, PlayerOptionsClass);
}

void APlayerControllerBase::OpenOptions()
{
	if (PlayerOptions != nullptr)
	{
		PlayerOptions->AddToViewport();

		FInputModeGameAndUI InputGameAndUI;

		InputGameAndUI.SetWidgetToFocus(PlayerOptions->TakeWidget());
		InputGameAndUI.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

		SetInputMode(InputGameAndUI);
		bShowMouseCursor = true;

		PlayerOptions->FocusButtonOnOpen();

		APlayerBase* BGPlayer = Cast<APlayerBase>(GetPawn());

		if (BGPlayer != nullptr)
		{
			BGPlayer->SetPlayerCanMove(false);

			BGPlayer->DisableAbilities();

			bIsOptionsOpen = true;
		}	
	}
}

void APlayerControllerBase::CloseOptions()
{
	if (PlayerOptions != nullptr)
	{
		PlayerOptions->RemoveFromParent();

		FInputModeGameOnly InputModeGame;

		SetInputMode(InputModeGame);
		bShowMouseCursor = false;

		APlayerBase* BGPlayer = Cast<APlayerBase>(GetPawn());

		if (BGPlayer != nullptr)
		{
			BGPlayer->EnableAbilities();
			BGPlayer->SetPlayerCanMove(true);

			bIsOptionsOpen = false;
		}
	}
}

void APlayerControllerBase::UpdateAllPlayerInfoServer_Implementation()
{
	UWorld* World = GetWorld();

	if (World != nullptr)
	{
		ABGGameState* GS = Cast<ABGGameState>(UGameplayStatics::GetGameState(World));

		if (GS != nullptr)
		{
			GS->UpdateAllPlayersHUDInfo();
		}
	}
}

bool APlayerControllerBase::UpdateAllPlayerInfoServer_Validate()
{
	return true;
}

void APlayerControllerBase::ToogleOptionsMenu()
{
	bIsOptionsOpen ? CloseOptions() : OpenOptions();
}

UPlayerHUDBase* APlayerControllerBase::GetPlayerHUD()
{
	return PlayerHUD;
}

void APlayerControllerBase::CreateEndGameScreen(bool IsVictory)
{
	if (EndScreen != nullptr)
	{ 
		return;
	}

	if (!DefeatScreenClass || !VictoryScreenClass) 
	{
		return;
	}

	if (!IsLocalPlayerController())
	{	
		return;
	}
	
	if (IsVictory)
	{
		EndScreen = CreateWidget<UEndGameScreenBase>(this, VictoryScreenClass);
		FInputModeUIOnly InputMode;
		SetInputMode(InputMode);
	}
	else
	{
		EndScreen = CreateWidget<UEndGameScreenBase>(this, DefeatScreenClass);

		FInputModeGameAndUI InputMode;

		SetInputMode(InputMode);

		bShowMouseCursor = true;
	}

	RemovePlayerHUD();

	EndScreen->AddToViewport();
}

void APlayerControllerBase::RemoveEndScreen()
{
	EndScreen->RemoveFromParent();

	EndScreen = nullptr;

	FInputModeGameOnly InputMode;

	SetInputMode(InputMode);
}

void APlayerControllerBase::DetachTheCamera()
{
	if (PlayerCameraManager != nullptr)
	{
		PlayerCameraManager = nullptr;
	}
}

void APlayerControllerBase::OnPossess(APawn* PawnToPossess)
{
	Super::OnPossess(PawnToPossess);

	APlayerStateBase* PS = GetPlayerState<APlayerStateBase>();
	if (PS)
	{
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, PawnToPossess);
	}

	AttachCamera();
}

void APlayerControllerBase::OnUnPossess()
{
	Super::OnUnPossess();

	RemovePlayerHUD();
}

void APlayerControllerBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	CreatePlayerHUD();
	AttachCamera();
}

void APlayerControllerBase::SetCharacterToSpawn_Implementation()
{
	UBGGameInstance* BGGameI = Cast<UBGGameInstance>(GetGameInstance());

	if (BGGameI != nullptr )
	{
		FPlayerInfo playerInfo = BGGameI->LocalPlayerInfo;

		switch (playerInfo.NetPlayerClass)
		{

		case EPlayerCharacterClass::Knight:
			ClassToSpawn = KnightClass;
			break;

		case EPlayerCharacterClass::Fighter:
			ClassToSpawn = FighterClass;
			break;

		case EPlayerCharacterClass::Ranger:
			ClassToSpawn = RangerClass;
			break;

		case EPlayerCharacterClass::Mage:
			ClassToSpawn = MageClass;
			break;

		default:
			break;
		}

		CallPlayerSpawn(ClassToSpawn, playerInfo.NetPlayerClass);
	}
}

void APlayerControllerBase::CallPlayerSpawn_Implementation(TSubclassOf<APlayerBase> CharacterSelected, EPlayerCharacterClass EPlayerClass)
{
	UWorld* World = GetWorld();

	if (World != nullptr)
	{
		ABGGameModeBase* BGGameMode = Cast<ABGGameModeBase>(World->GetAuthGameMode());

		if (BGGameMode != nullptr)
		{
			BGGameMode->SpawnPlayer(this, CharacterSelected, EPlayerClass);
		}
	}
}

FString APlayerControllerBase::GetPlayerNameByClass()
{
	UBGGameInstance* BGGameI = Cast<UBGGameInstance>(GetGameInstance());

	if (BGGameI != nullptr)
	{
		FPlayerInfo playerInfo = BGGameI->LocalPlayerInfo;

		switch (playerInfo.NetPlayerClass)
		{

		case EPlayerCharacterClass::Knight:
			return FString("Knight");
			break;

		case EPlayerCharacterClass::Fighter:
			return FString("Fighter");
			break;

		case EPlayerCharacterClass::Ranger:
			return FString("Ranger");
			break;

		case EPlayerCharacterClass::Mage:
			return FString("Mage");
			break;

		default:
			break;
		}
	}

	return FString("Default");
}

void APlayerControllerBase::PlayAbilityFailToActiveHUD(FGameplayTagContainer InTags)
{
	UPlayerHUDBase* PHUD = GetPlayerHUD();

	if (PHUD != nullptr)
	{
		PHUD->PlayAbilityFailToActive(InTags);
	}
}

void APlayerControllerBase::AttachCamera()
{
	if (PlayerCameraManager == nullptr)
	{
		return;
	}
	ABGPlayerCameraManager* CurrentCameraManager = Cast<ABGPlayerCameraManager>(PlayerCameraManager);
	APlayerBase* CurrentPlayer = Cast<APlayerBase>(GetPawn());

	if (CurrentCameraManager != nullptr && CurrentPlayer != nullptr)
	{
		CurrentCameraManager->OnPossess(CurrentPlayer);
	}
}

void APlayerControllerBase::BeginPlay()
{
	Super::BeginPlay();

	CreateOptionMenu();

	FInputModeGameOnly InputMode;

	SetInputMode(InputMode);

	bShowMouseCursor = false;
}
