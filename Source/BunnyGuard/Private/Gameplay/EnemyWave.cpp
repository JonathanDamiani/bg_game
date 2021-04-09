// Bunny Guard All Rigths.


#include "Gameplay/EnemyWave.h"
#include "Gameplay/SeedBase.h"
#include "Characters/Enemies/EnemyAIControllerBase.h"
#include "NavigationSystem.h"

void UEnemyWave::SpawnWave(const FRotator& SpawnRotation, const FVector& SpawnLocation, ASeedBase* InstigatingSeed)
{
	UWorld* World = GetWorld();

	if (World == nullptr)
	{
		return;
	}

	for (const TPair<TSubclassOf<AEnemyBase>, int32>& Pair : EnemyToSpawn)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		SpawnedEnemies = SpawnedEnemies + Pair.Value;
		for (int32 EnemyOfCount = 0; EnemyOfCount < Pair.Value; EnemyOfCount++)
		{
			const UNavigationSystemV1* const NavigationSystem = UNavigationSystemV1::GetCurrent(GetWorld());
			if (NavigationSystem == nullptr)
			{
				continue;
			}

			FNavLocation Result;
			const FVector Origin = SpawnLocation;

			bool bSuccess = false;
			do
			{
				bSuccess = NavigationSystem->GetRandomReachablePointInRadius(Origin, SpawnRadius, Result);
			} while (bSuccess == false);

			FVector SpawnVectorLocation(0.0f, 0.0f, 400.0f);
			SpawnVectorLocation = SpawnVectorLocation + Result.Location;

			AEnemyBase* SpawnedEnemy = World->SpawnActor<AEnemyBase>(Pair.Key, SpawnVectorLocation, SpawnRotation, SpawnParameters);
			SpawnedEnemy->OnCharacterDie.AddDynamic(this, &UEnemyWave::OnEnemyDeath);
			AEnemyAIControllerBase* EnemyController = Cast<AEnemyAIControllerBase>(SpawnedEnemy->GetController());
			EnemyController->SetSeed(InstigatingSeed);
		}
	}
}

void UEnemyWave::OnEnemyDeath(ACharacterBase* Character)
{
	if (IsValid(Character) == true)
	{
		DeathEnemies++;
		if (DeathEnemies == SpawnedEnemies)
		{
			OnWaveCompleted.Broadcast();
		}
	}
}
