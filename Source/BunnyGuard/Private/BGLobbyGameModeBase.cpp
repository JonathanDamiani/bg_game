// Copyright (C) 2020 Alejandro G�ereca Valdivia / Jonathan Dean Damiani

#include "BGLobbyGameModeBase.h"
#include "Characters/Player/LobbyPlayerController.h"
#include "BGGameInstance.h"
#include "HttpModule.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "BunnyGuardHttpRequestHandler.h"

void ABGLobbyGameModeBase::LaunchTheGame()
{
	UWorld* World = GetWorld();
	
	for (APlayerController* PC : AllPlayerControllers)
	{
		ALobbyPlayerController* LPC = Cast<ALobbyPlayerController>(PC);
		LPC->RemoveLobby();
	}
	
	if (World != nullptr)
	{
		UBGGameInstance* GI = Cast<UBGGameInstance>(World->GetGameInstance());
		BunnyGuardHttpRequestHandler::SendGameStart(GI->GameSessionId, ServerInfo);
		World->ServerTravel("/Game/Maps/Levels/Level_RealLevel?listen");
	}
}

void ABGLobbyGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	ALobbyPlayerController* LPC = Cast<ALobbyPlayerController>(NewPlayer);

	if (LPC != nullptr)
	{
		LPC->CreateLobby();
		AllPlayerControllers.Add(NewPlayer);
	}

	const UWorld* const World = GetWorld();

	if (World == nullptr)
	{
		return;
	}
	UBGGameInstance* GI = Cast<UBGGameInstance>(World->GetGameInstance());

	if (GI == nullptr)
	{
		return;
	}

	GI->ServerInfo.CurrentPlayers++;

	ServerInfo = GI->ServerInfo;
}
