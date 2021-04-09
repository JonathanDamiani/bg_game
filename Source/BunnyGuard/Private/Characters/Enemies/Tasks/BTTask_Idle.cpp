// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani

#include "Characters/Enemies/Tasks/BTTask_Idle.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
#include "AIController.h"

EBTNodeResult::Type UBTTask_Idle::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* const BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (BlackboardComponent == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	const APawn* const CurrentPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (CurrentPawn == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	const UNavigationSystemV1* const NavigationSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (NavigationSystem == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	FNavLocation Result;
	const FVector Origin = CurrentPawn->GetActorLocation();
	const bool bSuccess = NavigationSystem->GetRandomReachablePointInRadius(Origin, Radius, Result);

	if (bSuccess == false)
	{
		return EBTNodeResult::Failed;
	}

	BlackboardComponent->SetValueAsVector(DestinationVector.SelectedKeyName, Result.Location);
	return EBTNodeResult::Succeeded;
}
