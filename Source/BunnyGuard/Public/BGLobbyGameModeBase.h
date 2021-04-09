// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "PlayerInfo.h"
#include "AllServeInfo.h"
#include "Interfaces/IHttpRequest.h"
#include "BGLobbyGameModeBase.generated.h"


UCLASS()
class BUNNYGUARD_API ABGLobbyGameModeBase : public AGameMode
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable)
	void LaunchTheGame();

protected:
	virtual void PostLogin(APlayerController* NewPlayer) override;

	TArray<APlayerController*> AllPlayerControllers;

	int8 PlayerCount = 0;

	UPROPERTY(BlueprintReadWrite)
	FAllServeInfo ServerInfo;

	UPROPERTY(BlueprintReadWrite)
	TArray<FPlayerInfo> ClientsInfo;
};
