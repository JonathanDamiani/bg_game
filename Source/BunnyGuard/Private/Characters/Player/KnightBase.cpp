// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani

#include "Characters/Player/KnightBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Components/SkeletalMeshComponent.h"

AKnightBase::AKnightBase(const class FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

	SwordMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SwordMesh"));
	SwordMesh->SetupAttachment(GetMesh(), FName("DEF_Hand_R"));
	SwordMesh->SetSkeletalMesh(MainSwordMesh, false);

	ShieldMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ShieldMesh"));
	ShieldMesh->SetupAttachment(GetMesh(), FName("DEF_Hand_L"));
	SwordMesh->SetSkeletalMesh(MainShieldMesh, false);

	//////////////////////////////////////////////////////////////////////////
	// Basic Attack

	StartDistanceBasicAttackMultiplier = 200.0f;
	DistanceToTravelBasicAttackMultiplier = 300.0f;
	BasicAttackCollisionRadius = 200.0f;
	bIsShowingDebugBasicAttack = false;

	//////////////////////////////////////////////////////////////////////////
	// Charged Attack

	StartDistanceChargedAttackMultiplier = 1.0f;
	DistanceToTravelChargedAttackMultiplier = 1.0f;
	ChargedAttackCollisionRadius = 400.0f;
	bIsShowingDebugChargedAttack = false;

	//////////////////////////////////////////////////////////////////////////
	// Utility Skill

	 CurrentNumberOfHits = 0;
	 MaxNumberOfHits = 5;
	 DamageLevel = 1;
	 CanPlayerBlock = true;
	 IsPlayerBlocking = false;
	 IsMegaMode = false;
	 TimeToResetDamage = 3.0f;
	 TimeToResetHit = 1.0f;
}

void AKnightBase::ExecuteBasicAttack_Implementation()
{
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	SphereAttack(StartDistanceBasicAttackMultiplier, DistanceToTravelBasicAttackMultiplier, BasicAttackCollisionRadius, bIsShowingDebugBasicAttack);
}

void AKnightBase::ExecuteChargedAttack_Implementation()
{
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	SphereAttack(StartDistanceChargedAttackMultiplier, DistanceToTravelChargedAttackMultiplier, ChargedAttackCollisionRadius, bIsShowingDebugChargedAttack);
}

void AKnightBase::ExecuteUtility_Implementation()
{
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (IsMegaMode == false)
	{
		CurrentNumberOfHits++;
	}

	if (CurrentNumberOfHits >= MaxNumberOfHits)
	{
		IsMegaMode = true;
		CanPlayerBlock = false;

		SwapMeshes(MegaShieldMesh, MegaSwordMesh);

		FGameplayEventData EventData;

		FGameplayTag EventTag = FGameplayTag::RequestGameplayTag(FName("Event.Finish"));
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, EventTag, EventData);

		CurrentNumberOfHits = 0;

		DamageLevel = 2;

		GetWorldTimerManager().SetTimer(ResetBlock_TimerHandle, this, &AKnightBase::ExecuteResetBlock, TimeToResetDamage, false);
	}
}

void AKnightBase::ExecuteResetBlock()
{
	DamageLevel = 1;
	SwapMeshes(MainShieldMesh, MainSwordMesh);
	IsMegaMode = false;
}

void AKnightBase::StartResetHit()
{
	GetWorldTimerManager().SetTimer(ResetHit_TimerHandle, this, &AKnightBase::HandleResetHit, TimeToResetHit, true);
}

void AKnightBase::StopResetHit()
{
	GetWorldTimerManager().ClearTimer(ResetHit_TimerHandle);
}

void AKnightBase::HandleResetHit()
{
	if (IsPlayerBlocking)
	{
		return;
	}

	if (CurrentNumberOfHits <= 0)
	{
		StopResetHit();
		return;
	}

	CurrentNumberOfHits--;
}

void AKnightBase::SwapMeshes_Implementation(USkeletalMesh* Shield, USkeletalMesh* Sword)
{
	SwordMesh->SetSkeletalMesh(Sword, false);
	ShieldMesh->SetSkeletalMesh(Shield, false);
}
