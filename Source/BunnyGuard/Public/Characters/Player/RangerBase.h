// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani

#pragma once

#include "CoreMinimal.h"
#include "Characters/Player/PlayerBase.h"
#include "RangerBase.generated.h"

class AEnemyBase;
class USkeletalMeshComponent;
class ATargetBase;

UCLASS()
class BUNNYGUARD_API ARangerBase : public APlayerBase
{
	GENERATED_BODY()
public:
	ARangerBase(const class FObjectInitializer& ObjectInitializer);

	virtual void ExecuteCombat_Implementation() override;

	//////////////////////////////////////////////////////////////////////////
	// Ultimate Skill
	UFUNCTION(BlueprintCallable)
	void StartUltimateBehaviour();

	UFUNCTION(BlueprintCallable)
	void SetTargetForUltimate();

	UFUNCTION(BlueprintCallable)
	void ExecuteUltimateBehaviour();

	virtual void HandleAimPressed() override;

	virtual void HandleAimReleased() override;

protected:

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	USkeletalMeshComponent* BowMesh; 

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<ATargetBase> UltimateTarget;

	UPROPERTY()
	TArray<ATargetBase*> UltimateTargetsReference;
	
	//////////////////////////////////////////////////////////////////////////
	// Combat Skill

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Abilites/Combat Attack")
	float CombatSkillShootingRatio;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Abilites/Combat Attack")
	float CombatSkillDuration;

	FTimerHandle TimeHandle_CombatSkillCancel;
	FTimerHandle TimeHandle_CombatSkillShooting;

	UFUNCTION(BlueprintImplementableEvent)
	void StartShooting();
	void StartShooting_Implementation();

	UFUNCTION(BlueprintCallable)
	void StopShooting();

	//////////////////////////////////////////////////////////////////////////
	// Ultimate Skill
	UPROPERTY(BlueprintReadWrite)
	TArray<AEnemyBase*> TargetEnemiesForUltimate;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Abilites/Ultimate Attack")
	bool bEnableDebugUltimate;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Abilites/Ultimate Attack")
	float UltimateRadiusToCheck;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Abilites/Ultimate Attack")
	float UltimateRangeToCheck;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Abilites/Ultimate Attack")
	int32 MaxNumberOfTargetsForUltimate;

	UPROPERTY(BlueprintReadWrite)
	int32 CurrentNumberOfTargetsForUltimate;

	UPROPERTY()
	bool bInUltimateExecutionMode;

	FTimerHandle UltimateGetTarget_TimerHandle;

	UFUNCTION(Client, Reliable)
	void SetupVisualTargetsUltimate(AEnemyBase* EnemyTarget);
	void SetupVisualTargetsUltimate_Implementation(AEnemyBase* EnemyTarget);
};
