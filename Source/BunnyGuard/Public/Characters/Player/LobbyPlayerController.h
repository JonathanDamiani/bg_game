// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerInfo.h"
#include "Net/UnrealNetwork.h"
#include "LobbyPlayerController.generated.h"

class UUserWidget;
class ULobbyMenuBase;

UCLASS()
class BUNNYGUARD_API ALobbyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UFUNCTION(Client, Reliable)
	void CreateLobby();
	void CreateLobby_Implementation();

	UFUNCTION(Client, Reliable)
	void RemoveLobby();
	void RemoveLobby_Implementation();

	UFUNCTION(BlueprintCallable)
	ULobbyMenuBase* GetLobbyWidget() const;

protected:
	UPROPERTY(EditAnywhere, Category = "LobbyWidget")
	TSubclassOf<UUserWidget> LobbyUserWidget;

	ULobbyMenuBase* LobbyMenu;

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void SaveLocalPlayerInfo(FPlayerInfo infoToSave);
	void SaveLocalPlayerInfo_Implementation(FPlayerInfo infoToSave);

};
