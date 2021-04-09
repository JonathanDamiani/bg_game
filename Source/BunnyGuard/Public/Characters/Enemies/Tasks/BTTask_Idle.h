// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Idle.generated.h"

/**
 * Node to enable idle behaviour for enemies
 */
UCLASS()
class BUNNYGUARD_API UBTTask_Idle : public UBTTaskNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Blackboard)
	FBlackboardKeySelector DestinationVector;

	UPROPERTY(EditAnywhere, Category = Parameters)
	float Radius = 300.0f;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
