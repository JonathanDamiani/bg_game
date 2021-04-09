// Bunny Guard All Rigths.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Characters/Enemies/EnemyBase.h"
#include "EnemyWave.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWaveCompleted);

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, DefaultToInstanced, EditInlineNew)
class BUNNYGUARD_API UEnemyWave : public UObject
{
	GENERATED_BODY()

public:
	FWaveCompleted OnWaveCompleted;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<TSubclassOf<AEnemyBase>, int32> EnemyToSpawn;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float SpawnRadius = 200.0f;

	void SpawnWave(const FRotator& SpawnRotation, const FVector& SpawnLocation, class ASeedBase* InstigatingSeed);

	UFUNCTION()
	void OnEnemyDeath(ACharacterBase* Character);

private:
	int32 SpawnedEnemies = 0;

	int32 DeathEnemies = 0;
};
