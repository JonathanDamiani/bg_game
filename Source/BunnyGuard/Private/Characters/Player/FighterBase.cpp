// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani

#include "Characters/Player/FighterBase.h"
#include "Characters/Enemies/EnemyBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"

AFighterBase::AFighterBase(const class FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	//////////////////////////////////////////////////////////////////////////
	// Basic Attack

	StartDistanceBasicAttackMultiplier = 200.0f;
	DistanceToTravelBasicAttackMultiplier = 300.0f;
	BasicAttackCollisionRadius = 200.0f;
	bIsShowingDebugBasicAttack = false;

	//////////////////////////////////////////////////////////////////////////
	// Charged Attack

	StartDistanceChargedAttackMultiplier = 200.0f;
	DistanceToTravelChargedAttackMultiplier = 600.0f;
	ChargedAttackCollisionRadius = 200.0f;
	bIsShowingDebugChargedAttack = false;

	//////////////////////////////////////////////////////////////////////////
	// Utility Attack

	UtilityCollider = CreateDefaultSubobject<USphereComponent>(TEXT("UtilityCollider"));
	UtilityCollider->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	UtilityCollider->InitSphereRadius(300.0f);

	UtilityCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//////////////////////////////////////////////////////////////////////////
	// Combat Attack

	StartDistanceCombatAttackMultiplier = 0.0f;
	DistanceToTravelCombatAttackMultiplier = 0.0f;
	CombatAttackCollisionRadius = 400.0f;
	bIsShowingDebugCombatAttack = false;

	//////////////////////////////////////////////////////////////////////////
	// Ultimate Attack

	StartDistanceUltimateAttackMultiplier = 0.0f;
	DistanceToTravelUltimateAttackMultiplier = 0.0f;
	UltimateAttackCollisionRadius = 400.0f;
	bIsShowingDebugUltimateAttack = false;
	NumberOfHitsUltimate = 5;
	CurrentNumberOfHitsUltimate = 0;
}


void AFighterBase::BeginPlay()
{
	Super::BeginPlay();

	UtilityCollider->OnComponentBeginOverlap.AddDynamic(this, &AFighterBase::OnUtilityOverlap);

}

void AFighterBase::ExecuteChargedAttack_Implementation()
{
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	SphereAttack(StartDistanceChargedAttackMultiplier, DistanceToTravelChargedAttackMultiplier, ChargedAttackCollisionRadius, bIsShowingDebugChargedAttack);
}

void AFighterBase::ExecuteBasicAttack_Implementation()
{
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	SphereAttack(StartDistanceBasicAttackMultiplier, DistanceToTravelBasicAttackMultiplier, BasicAttackCollisionRadius, bIsShowingDebugBasicAttack);
}

//////////////////////////////////////////////////////////////////////////
// Utility Skill 

void AFighterBase::ExecuteUtility_Implementation()
{
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	SendUtilityEventBegin();

	UtilityCollider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

}

void AFighterBase::OnUtilityOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr)
	{
		return;
	}

	AEnemyBase* EnemyToDamage = Cast<AEnemyBase>(OtherActor);

	if (EnemyToDamage != nullptr)
	{
		HandleDamageExecutionForUtility(EnemyToDamage);
	}
}

void AFighterBase::SendUtilityEventBegin()
{
	FGameplayEventData EventData;

	FGameplayTag EventTag = FGameplayTag::RequestGameplayTag(FName("Event.Execute"));
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, EventTag, EventData);
}

void AFighterBase::HandleDamageExecutionForUtility_Implementation(AActor* ActorToDamage)
{
	FGameplayEventData EventData;
	EventData.Target = ActorToDamage;

	FGameplayTag EventTag = FGameplayTag::RequestGameplayTag(FName("Data.Damage"));
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, EventTag, EventData);
}

void AFighterBase::ResetUtilityOnEnd()
{
	UtilityCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
}

//////////////////////////////////////////////////////////////////////////
// Combat Skill 

void AFighterBase::ExecuteCombat_Implementation()
{
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	SphereAttack(StartDistanceCombatAttackMultiplier, DistanceToTravelCombatAttackMultiplier, CombatAttackCollisionRadius, bIsShowingDebugCombatAttack);
}


void AFighterBase::ExecuteUltimate_Implementation()
{
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	UAnimMontage* CurrentMontage = GetMesh()->GetAnimInstance()->GetCurrentActiveMontage();

	if (CurrentMontage == nullptr)
	{
		return;
	}

	float StartTimeDamage;
	float FinalTimeDamage;

	int32 SectionIndex = CurrentMontage->GetSectionIndex(FName("DamageSection"));
	CurrentMontage->GetSectionStartAndEndTime(SectionIndex, StartTimeDamage, FinalTimeDamage);

	float TotalDamageTime = FinalTimeDamage - StartTimeDamage;
	float DamageRatioUltimate = TotalDamageTime / NumberOfHitsUltimate;

	GetWorldTimerManager().SetTimer(UltimateAttack_TimerHandle, this, &AFighterBase::UltimateAttackAction, DamageRatioUltimate, true);
}

void AFighterBase::UltimateAttackAction()
{
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	SphereAttack(StartDistanceUltimateAttackMultiplier, DistanceToTravelUltimateAttackMultiplier, UltimateAttackCollisionRadius, bIsShowingDebugUltimateAttack);

	CurrentNumberOfHitsUltimate ++;

	if (CurrentNumberOfHitsUltimate >= NumberOfHitsUltimate)
	{
		GetWorldTimerManager().ClearTimer(UltimateAttack_TimerHandle);
		CurrentNumberOfHitsUltimate = 0;
	}
}
