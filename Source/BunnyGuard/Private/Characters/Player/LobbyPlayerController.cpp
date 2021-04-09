// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani

#include "Characters/Player/LobbyPlayerController.h"
#include "UI/LobbyMenuBase.h"
#include "Kismet/GameplayStatics.h"
#include "Characters/Player/PlayerStateBase.h"
#include "BGGameInstance.h"
#include "BGLobbyGameModeBase.h"

void ALobbyPlayerController::CreateLobby_Implementation()
{
	if (LobbyUserWidget == nullptr) 
	{
		return;
	}

	LobbyMenu = CreateWidget<ULobbyMenuBase>(this, LobbyUserWidget);

	LobbyMenu->AddToViewport();

	FInputModeGameAndUI InputModeGameUI;

	InputModeGameUI.SetWidgetToFocus(LobbyMenu->TakeWidget());
	InputModeGameUI.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	SetInputMode(InputModeGameUI);

	bShowMouseCursor = true;
}

void ALobbyPlayerController::RemoveLobby_Implementation()
{
	if (LobbyMenu == nullptr)
	{
		return;
	}

	if (!IsLocalPlayerController())
	{
		return;
	}

	LobbyMenu->RemoveFromParent();
	LobbyMenu = nullptr;
}

ULobbyMenuBase* ALobbyPlayerController::GetLobbyWidget() const
{
	return LobbyMenu;
}

void ALobbyPlayerController::SaveLocalPlayerInfo_Implementation(FPlayerInfo infoToSave)
{
	UBGGameInstance* BGGameI = Cast<UBGGameInstance>(GetGameInstance());

	if (BGGameI != nullptr)
	{
		BGGameI->LocalPlayerInfo = infoToSave;
	}
}

