// Bunny Guard All Rigths.


#include "EQSTests/EnvQueryTest_PlayerIsAlive.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_ActorBase.h"
#include "EnvironmentQuery/Contexts/EnvQueryContext_Querier.h"
#include "Characters/Player/PlayerBase.h"

UEnvQueryTest_PlayerIsAlive::UEnvQueryTest_PlayerIsAlive()
{
	PlayerIsAliveContext = UEnvQueryContext_Querier::StaticClass();
	Cost = EEnvTestCost::Low;
	ValidItemType = UEnvQueryItemType_ActorBase::StaticClass();
	SetWorkOnFloatValues(false);
}

void UEnvQueryTest_PlayerIsAlive::RunTest(FEnvQueryInstance& QueryInstance) const
{
	UObject* QueryOwner = QueryInstance.Owner.Get();
	if (QueryOwner == nullptr)
	{
		return;
	}

	TArray<AActor*> ContextActors;
	if (!QueryInstance.PrepareContext(PlayerIsAliveContext, ContextActors))
	{
		return;
	}

	for (FEnvQueryInstance::ItemIterator It(this, QueryInstance); It; ++It)
	{
		const bool bIsAlive = false;

		APlayerBase* CurrentPlayerReference = Cast<APlayerBase>(GetItemActor(QueryInstance, It.GetIndex()));

		It.SetScore(TestPurpose, FilterType, CurrentPlayerReference->IsAlive(), true);
	}
}

FText UEnvQueryTest_PlayerIsAlive::GetDescriptionTitle() const
{
	return FText::FromString(FString::Printf(TEXT("Player Is Alive")));
}

FText UEnvQueryTest_PlayerIsAlive::GetDescriptionDetails() const
{
	return DescribeFloatTestParams();
}
