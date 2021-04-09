// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani

#include "Characters/Enemies/EnemyAIControllerBase.h"
#include "Characters/Enemies/EnemyBase.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Gameplay/SeedBase.h"
#include "Navigation/CrowdFollowingComponent.h"

AEnemyAIControllerBase::AEnemyAIControllerBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{

}


void AEnemyAIControllerBase::OnPossess(APawn* TargetPawn)
{
	Super::OnPossess(TargetPawn);

	const AEnemyBase* const ControlledEnemy = Cast<AEnemyBase>(TargetPawn);

	if (ControlledEnemy == nullptr)
	{
		return;
	}
	const EEnemyType EnemyType = ControlledEnemy->GetType();

	if (BehaviourTrees.Contains(EnemyType) == false)
	{
		return;
	}
	UBehaviorTree* TreeToRun = BehaviourTrees[EnemyType];
	
	if (TreeToRun == nullptr)
	{
		return;
	}

	RunBehaviorTree(TreeToRun);
}

void AEnemyAIControllerBase::SetSeed(ASeedBase* Seed)
{
	if (Blackboard != nullptr)
	{
		Blackboard->SetValueAsObject(FName("Seed"), Seed);
	}

	InstigatingSeed = Seed;
}

void AEnemyAIControllerBase::NofifySeed()
{
	if (InstigatingSeed == nullptr)
	{
		return;
	}
}
