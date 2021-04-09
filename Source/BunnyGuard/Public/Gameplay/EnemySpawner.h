// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/EngineTypes.h"
#include "Gameplay/EnemyWave.h"
#include "EnemySpawner.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSpawnerDone);

class AEnemyBase;

UCLASS()
class BUNNYGUARD_API AEnemySpawner : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AEnemySpawner();

	FSpawnerDone OnSpawnerDone;

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Spawn(class ASeedBase* CurrentSeed);
	void Spawn_Implementation(class ASeedBase* CurrentSeed);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SpawnWave();
	void SpawnWave_Implementation();

	UFUNCTION()
	void OnWaveCompleted();

protected:
	UPROPERTY(Instanced, BlueprintReadWrite, EditAnywhere, Category = "Spawn Properties")
	TArray<UEnemyWave*> EnemyToSpawn;

	class ASeedBase* InstigatingSeed;

	int32 WaveIndex = 0;
};
