// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani

#include "Characters/Player/RangerBase.h"
#include "Characters/Enemies/EnemyBase.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/TargetBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/EngineTypes.h"

ARangerBase::ARangerBase(const class FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	CombatSkillShootingRatio = 0.1f;
	CombatSkillDuration = 3.0f;
	bInUltimateExecutionMode = false;

	BowMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BowMesh"));
	BowMesh->SetupAttachment(GetMesh(), FName("DEF_Hand_L"));

	MaxNumberOfTargetsForUltimate = 2;
	UltimateRadiusToCheck = 50.0f;
	UltimateRangeToCheck = 3000.0f;	
	bEnableDebugUltimate = true;
}

void ARangerBase::StartUltimateBehaviour()
{
	HandleAimPressed();
	bInUltimateExecutionMode = true;
	GetWorldTimerManager().SetTimer(UltimateGetTarget_TimerHandle, this, &ARangerBase::SetTargetForUltimate, 0.2f, true);
}

void ARangerBase::SetTargetForUltimate()
{
	if (bInUltimateExecutionMode && CurrentNumberOfTargetsForUltimate < MaxNumberOfTargetsForUltimate)
	{
		UWorld* World = GetWorld();

		if (World == nullptr)
		{
			return;
		}

		FVector StartPositionOfRay = FollowCamera->GetComponentLocation();
		FVector EndPositionOfRay = (FollowCamera->GetForwardVector() * UltimateRangeToCheck) + StartPositionOfRay;

		TArray<FHitResult> OutHits;

		TArray<AActor*> IgnoreActors;
		IgnoreActors.Add(this);

		if (bEnableDebugUltimate)
		{
			UKismetSystemLibrary::SphereTraceMulti(this, StartPositionOfRay, EndPositionOfRay, UltimateRadiusToCheck, UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel1), false, IgnoreActors, EDrawDebugTrace::ForDuration, OutHits, true);
		}
		else
		{
			UKismetSystemLibrary::SphereTraceMulti(this, StartPositionOfRay, EndPositionOfRay, UltimateRadiusToCheck, UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel1), false, IgnoreActors, EDrawDebugTrace::None, OutHits, true);
		}

		if (!OutHits.Num())
		{
			return;
		}

		for (FHitResult OutHit : OutHits)
		{
			AEnemyBase* EnemyHitted = Cast<AEnemyBase>(OutHit.GetActor());

			if (EnemyHitted != nullptr)
			{
				if (!TargetEnemiesForUltimate.Contains(EnemyHitted))
				{
					
					CurrentNumberOfTargetsForUltimate++;
					TargetEnemiesForUltimate.Add(EnemyHitted);

					SetupVisualTargetsUltimate(EnemyHitted);
				}
			}
		}
	}
}


void ARangerBase::SetupVisualTargetsUltimate_Implementation(AEnemyBase* EnemyTarget)
{
	UWorld* World = GetWorld();

	if (World == nullptr)
	{
		return;
	}

	if (UltimateTarget != nullptr)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		ATargetBase* TargetActor = World->SpawnActor<ATargetBase>(UltimateTarget, EnemyTarget->GetActorLocation(), EnemyTarget->GetActorRotation(), SpawnParameters);
		TargetActor->SetTargetReference(EnemyTarget);
		TargetActor->SetBaseReference(this);
		TargetActor->EnableReposition();
		UltimateTargetsReference.Add(TargetActor);
	}
}

void ARangerBase::ExecuteUltimateBehaviour()
{
	for (ATargetBase* TargetToDestroy : UltimateTargetsReference)
	{
		if (TargetToDestroy != nullptr)
		{
			TargetToDestroy->Destroy();
		}
	}

	bInUltimateExecutionMode = false;
	HandleAimReleased();
	GetWorldTimerManager().ClearTimer(UltimateGetTarget_TimerHandle);
}

void ARangerBase::HandleAimPressed()
{
	if (!bInUltimateExecutionMode)
	{
		Super::HandleAimPressed();
	}
}

void ARangerBase::HandleAimReleased()
{
	if (!bInUltimateExecutionMode)
	{
		Super::HandleAimReleased();
	}
}

//////////////////////////////////////////////////////////////////////////
// Combat Skill
void ARangerBase::StartShooting_Implementation()
{
	// Implemented in Blueprints
}

void ARangerBase::StopShooting()
{
	GetWorldTimerManager().ClearTimer(TimeHandle_CombatSkillShooting);

	FGameplayEventData EventData;

	FGameplayTag EventTag = FGameplayTag::RequestGameplayTag(FName("Event.Finish"));
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, EventTag, EventData);
}

void ARangerBase::ExecuteCombat_Implementation()
{
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	UWorld* World = GetWorld();

	GetWorldTimerManager().SetTimer(TimeHandle_CombatSkillCancel, this, &ARangerBase::StopShooting, CombatSkillDuration, false);
	GetWorldTimerManager().SetTimer(TimeHandle_CombatSkillShooting, this, &ARangerBase::StartShooting, CombatSkillShootingRatio, true);
}	
