// Copyright (C) 2020 Alejandro G�ereca Valdivia / Jonathan Dean Damiani

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "PlayerInfo.h"
#include "Interfaces/IHttpRequest.h"
#include "BGGameModeBase.generated.h"

class APlayerStart;
class APlayerBase;
class APlayerControllerBase;

UCLASS()
class BUNNYGUARD_API ABGGameModeBase : public AGameMode
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SwapCharacter(APlayerController* PlayerController, TSubclassOf<APlayerBase> CharacterToSwap);

	UFUNCTION(BlueprintCallable)
	void SpawnPlayer(APlayerController* PlayerController, TSubclassOf<APlayerBase> PlayerToSpawn, EPlayerCharacterClass EPlayerClass);

	UFUNCTION(BlueprintCallable)
	void CheckDefeatCondition();

	UFUNCTION(BlueprintCallable)
	void CheckVictoryCondition();

	UFUNCTION(BlueprintCallable)
	void ActiveEndGameScreen(bool bIsVictory);

	UFUNCTION(BlueprintCallable)
	void EnterCombatState(const bool bIsBoss);

	UFUNCTION(BlueprintCallable)
	void ExitCombatState();

	UPROPERTY(BlueprintReadWrite)
	TArray<APlayerControllerBase*> AllBGController;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bIsInTestMode = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bCombatMusicIsPlaying = false;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString TestModeStartPosition;

	virtual void GenericPlayerInitialization(AController* C);

	UFUNCTION(BlueprintCallable)
	void TravelToLevel(FString LevelToTravel);

	UFUNCTION(BlueprintCallable)
	void RestartLevel();
};
