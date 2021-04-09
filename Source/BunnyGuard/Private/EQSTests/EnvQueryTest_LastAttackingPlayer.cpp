// Bunny Guard All Rigths.


#include "EQSTests/EnvQueryTest_LastAttackingPlayer.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_ActorBase.h"
#include "EnvironmentQuery/Contexts/EnvQueryContext_Querier.h"
#include "Characters/Enemies/EnemyBase.h"
#include "Characters/CharacterBase.h"

UEnvQueryTest_LastAttackingPlayer::UEnvQueryTest_LastAttackingPlayer()
{
	LastAttackingPlayer = UEnvQueryContext_Querier::StaticClass();
	Cost = EEnvTestCost::Low;
	ValidItemType = UEnvQueryItemType_ActorBase::StaticClass();
	SetWorkOnFloatValues(false);
}

void UEnvQueryTest_LastAttackingPlayer::RunTest(FEnvQueryInstance& QueryInstance) const
{
	UObject* QueryOwner = QueryInstance.Owner.Get();
	if (QueryOwner == nullptr)
	{
		return;
	}

	TArray<AActor*> ContextActors;
	if (!QueryInstance.PrepareContext(LastAttackingPlayer, ContextActors))
	{
		return;
	}

	for (int32 ContextIndex = 0; ContextIndex < ContextActors.Num(); ContextIndex++)
	{
		AActor* const ActorReference = ContextActors[ContextIndex];
		const AEnemyBase* const CurrentEnemy = Cast<AEnemyBase>(ActorReference);

		ACharacterBase* LastAttackingPlayerCharacter = CurrentEnemy->GetLastAttackingCharacter();
		
		for (FEnvQueryInstance::ItemIterator It(this, QueryInstance); It; ++It)
		{
			bool bIsLastAttackingPlayer = false;

			AActor* CurrentPlayerReference = GetItemActor(QueryInstance, It.GetIndex());
			if (CurrentPlayerReference == LastAttackingPlayerCharacter)
			{
				bIsLastAttackingPlayer = true;
			}

			It.SetScore(TestPurpose, FilterType, bIsLastAttackingPlayer, true);
		}
	}

}

FText UEnvQueryTest_LastAttackingPlayer::GetDescriptionTitle() const
{
	return FText::FromString(FString::Printf(TEXT("Is Last Attacking Player")));
}

FText UEnvQueryTest_LastAttackingPlayer::GetDescriptionDetails() const
{
	return DescribeFloatTestParams();
}
