// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Characters/Enemies/Utils/EnemyType.h"
#include "EnemyAIControllerBase.generated.h"

/**
 * 
 */
UCLASS()
class BUNNYGUARD_API AEnemyAIControllerBase : public AAIController
{
	GENERATED_BODY()

public:
	AEnemyAIControllerBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
	TMap<EEnemyType, class UBehaviorTree*> BehaviourTrees;

	void SetSeed(class ASeedBase* Seed);

	void NofifySeed();

protected:
	virtual void OnPossess(APawn* Pawn) override;

	class ASeedBase* InstigatingSeed;
};
