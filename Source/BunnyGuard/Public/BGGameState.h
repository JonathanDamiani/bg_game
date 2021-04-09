// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "BGGameState.generated.h"


UCLASS()
class BUNNYGUARD_API ABGGameState : public AGameState
{
	GENERATED_BODY()

public:

	UFUNCTION(NetMulticast, Unreliable)
	void SetAllPlayersHUDInfo();
	void SetAllPlayersHUDInfo_Implementation();

	UFUNCTION(NetMulticast, Unreliable)
	void UpdateAllPlayersHUDInfo();
	void UpdateAllPlayersHUDInfo_Implementation();

	UFUNCTION(BlueprintCallable, NetMulticast, Unreliable)
	void UpdateAllPlayersQuestInfo(const FString& QuestText);
	void UpdateAllPlayersQuestInfo_Implementation(const FString& QuestText);

	UFUNCTION(BlueprintCallable, NetMulticast, Unreliable)
	void UpdateBossHealth(float BossHealth);
	void UpdateBossHealth_Implementation(float BossHealth);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void ResetPlayersToRestart();
	void ResetPlayersToRestart_Implementation();

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void UpdateSeedTreeUI();
	void UpdateSeedTreeUI_Implementation();

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void SendEndCondition(bool bIsVictory);
	void SendEndCondition_Implementation(bool bIsVictory);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void TeleportPlayersToPosition(const TArray<FTransform>& TransformToTeleport);
	void TeleportPlayersToPosition_Implementation(const TArray<FTransform>& TransformToTeleport);
};
