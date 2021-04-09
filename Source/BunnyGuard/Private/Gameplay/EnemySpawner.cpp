// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani


#include "Gameplay/EnemySpawner.h"
#include "Characters/Enemies/EnemyBase.h"
#include "Characters/Enemies/EnemyAIControllerBase.h"
#include "Gameplay/SeedBase.h"

// Sets default values
AEnemySpawner::AEnemySpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
}

void AEnemySpawner::Spawn_Implementation(ASeedBase* CurrentSeed)
{
	InstigatingSeed = CurrentSeed;
	SpawnWave();
}

void AEnemySpawner::SpawnWave_Implementation()
{
	if (WaveIndex < EnemyToSpawn.Num())
	{
		UEnemyWave* Wave = EnemyToSpawn[WaveIndex];
		Wave->SpawnWave(GetActorRotation(), GetActorLocation(), InstigatingSeed);
		Wave->OnWaveCompleted.AddDynamic(this, &AEnemySpawner::OnWaveCompleted);
	}
}

void AEnemySpawner::OnWaveCompleted()
{
	WaveIndex++;
	if (WaveIndex == EnemyToSpawn.Num())
	{
		OnSpawnerDone.Broadcast();
		return;
	}

	SpawnWave();
}
