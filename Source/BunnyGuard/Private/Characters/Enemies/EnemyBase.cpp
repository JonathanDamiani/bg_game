// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani

#include "Characters/Enemies/EnemyBase.h"
#include "Characters/Player/PlayerBase.h"
#include "Characters/Enemies/EnemyAIControllerBase.h"
#include "AbilitySystem/BGAbilitySystemComponent.h"
#include "AbilitySystem/AttributeSetBase.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/FloatingHUDBase.h"
#include "BGGameModeBase.h"
#include "AbilitySystem/TornadoBase.h"
#include "AbilitySystem/VortexBase.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AkAudioEvent.h"
#include "AkComponent.h"
#include "AkGameplayStatics.h"
#include "Niagara/Public/NiagaraComponent.h"
#include "Niagara/Public/NiagaraFunctionLibrary.h"

AEnemyBase::AEnemyBase(const class FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	AbilitySystemComponent = CreateDefaultSubobject<UBGAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSetBase = CreateDefaultSubobject<UAttributeSetBase>(TEXT("AttributeSetBase"));

	RotationMovementComponent = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotationController"));
	RotationMovementComponent->RotationRate = FRotator(0, 0, 0);

	SelfRotationMovementComponent = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("SelfRotationController"));
	SelfRotationMovementComponent->RotationRate = FRotator(0, 0, 0);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	FloatingHUDComponent = CreateDefaultSubobject<UWidgetComponent>(FName("FloatingHUDComponent"));
	FloatingHUDComponent->SetupAttachment(RootComponent);
	FloatingHUDComponent->SetRelativeLocation(FVector(0, 0, 120));
	FloatingHUDComponent->SetWidgetSpace(EWidgetSpace::Screen);
	FloatingHUDComponent->SetDrawSize(FVector2D(500, 500));
	FloatingHUDComponent->SetVisibility(false);

	RagdollTag = FGameplayTag::RequestGameplayTag(FName("State.RagDoll"));
}

void AEnemyBase::HealthChanged(const FOnAttributeChangeData& Data)
{
	float Health = Data.NewValue;

	if (!IsAlive())
	{
		Die();
	}
}

EEnemyType AEnemyBase::GetType() const
{
	return EnemyType;
}

void AEnemyBase::SetTornadoReference(ATornadoBase* TornadoReference)
{
	TornadoTempReference = TornadoReference;	
}

ATornadoBase* AEnemyBase::GetTornadoReference() const
{
	if (TornadoTempReference != nullptr)
	{
		return TornadoTempReference;
	}
	return nullptr;
}

void AEnemyBase::StartTornadoBehaviour()
{
	if (GetTornadoReference() != nullptr)
	{
		if (AbilitySystemComponent.IsValid())
		{
			AbilitySystemComponent->AddLooseGameplayTag(RagdollTag);
		}

		USkeletalMeshComponent* CharMesh = GetMesh();

		CapsuleInitialRotation = GetCapsuleComponent()->GetComponentRotation();

		CharMesh->SetAllBodiesBelowSimulatePhysics(FName("DEF_Hip"), true, false);

		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

		
		FVector PivotOfRotation = GetTornadoReference()->GetActorLocation();
		PivotOfRotation.Normalize();

		float rotationRadius = FMath::RandRange(300.f, 600.0f);

		PivotOfRotation *= rotationRadius;
		 
		RotationMovementComponent->PivotTranslation = PivotOfRotation;

		float rotationSpeed = FMath::RandRange(120.0f, 240.0f);

		float selfRotationSpeed = FMath::RandRange(90.0f, 180.0f);

		RotationMovementComponent->RotationRate = FRotator(0, rotationSpeed, 0);
		SelfRotationMovementComponent->RotationRate = FRotator(selfRotationSpeed, selfRotationSpeed, selfRotationSpeed);
	}
}

void AEnemyBase::EndTornadoBehaviour()
{
	USkeletalMeshComponent* CharMesh = GetMesh();

	CharMesh->SetAllBodiesBelowSimulatePhysics(FName("DEF_Hip"), false, false);

	RotationMovementComponent->RotationRate = FRotator(0, 0, 0);
	SelfRotationMovementComponent->RotationRate = FRotator(0, 0, 0);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	GetCapsuleComponent()->SetRelativeRotation(CapsuleInitialRotation);

	if (AbilitySystemComponent.IsValid())
	{
		AbilitySystemComponent->RemoveLooseGameplayTag(RagdollTag);
		AbilitySystemComponent->SetTagMapCount(RagdollTag, 0);
	}

}

void AEnemyBase::SetVortexReference(AVortexBase* VortexReference)
{
	VortexTempReference = VortexReference;
}

AVortexBase* AEnemyBase::GetVortexReference() const
{
	return VortexTempReference;
}

void AEnemyBase::StartVortexBehaviour()
{
	if (GetVortexReference() != nullptr)
	{
		if (AbilitySystemComponent.IsValid())
		{
			AbilitySystemComponent->AddLooseGameplayTag(RagdollTag);
		}

		USkeletalMeshComponent* CharMesh = GetMesh();

		CharMesh->SetAllBodiesBelowSimulatePhysics(FName("DEF_Hip"), true, false);

		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	}
}

void AEnemyBase::EndVortexBehaviour()
{
	USkeletalMeshComponent* CharMesh = GetMesh();

	CharMesh->SetAllBodiesBelowSimulatePhysics(FName("DEF_Hip"), false, false);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

	if (AbilitySystemComponent.IsValid())
	{
		AbilitySystemComponent->RemoveLooseGameplayTag(RagdollTag);
		AbilitySystemComponent->SetTagMapCount(RagdollTag, 0);
	}
}

void AEnemyBase::ExecuteEnemyAbility(const bool bIsBasic) const
{
	TSubclassOf<UGameplayAbility> AbilityToActivate = bIsBasic == true ? BasicAbility : CombatAbility;
	EnemyAbilitySystemComponent->TryActivateAbilityByClass(AbilityToActivate);
}

void AEnemyBase::BlockAttack()
{
	if (bIsBlocking == false)
	{
		ExecuteEnemyAbility(false);
		bIsBlocking = true;
	}

	if (BlockEvent != nullptr)
	{
		FOnAkPostEventCallback NullCallback;
		TArray<FAkExternalSourceInfo> External;
		AkAudioComponent->PostAkEvent(BlockEvent, int32(0), NullCallback, External, BlockEvent->GetName());
	}

	GetWorldTimerManager().SetTimer(ResetBlock_TimerHandle, this, &AEnemyBase::FinalizeBlock, TimeToLowerShield, false);
}

void AEnemyBase::FinalizeBlock()
{
	FGameplayEventData EventData;
	FGameplayTag EventTag = FGameplayTag::RequestGameplayTag(FName("Event.Finish"));
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, EventTag, EventData);
	bIsBlocking = false;
}

bool AEnemyBase::CanBlock() const
{
	return bCanBlock;
}

// TODO: Change to single function with enum mapping
void AEnemyBase::SetStraffingSpeed()
{
	GetCharacterMovement()->MaxWalkSpeed = StrafeSpeed;
}

void AEnemyBase::SetRepositionSpeed()
{
	GetCharacterMovement()->MaxWalkSpeed = RepositionSpeed;
}

void AEnemyBase::SetSprintSpeed()
{
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void AEnemyBase::ResetSpeed()
{
	GetCharacterMovement()->MaxWalkSpeed = DefaultSpeed;
}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	SpawnDefaultController();

	DefaultSpeed = GetCharacterMovement()->MaxWalkSpeed;

	if (AbilitySystemComponent.IsValid())
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		InitializeAttributes();
		AddStartupEffects();
		AddCharacterAbilities();

		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);

		if (PC != nullptr && PC->IsLocalPlayerController())
		{
			if (FloatingHUDClass != nullptr)
			{
				FloatingHUD = CreateWidget<UFloatingHUDBase>(PC, FloatingHUDClass);
				if (FloatingHUD != nullptr && FloatingHUDComponent != nullptr)
				{
					FloatingHUDComponent->SetWidget(FloatingHUD);

					FloatingHUD->SetHealthPercentage(GetHealth() / GetMaxHealth());
				}
			}
		}
	}

	HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthAttribute()).AddUObject(this, &AEnemyBase::HealthChanged);
}

void AEnemyBase::Die()
{
	if (bIsPendingDeath == true)
	{
		return;
	}

	Super::Die();

	bIsPendingDeath = true;

	UWorld* World = GetWorld();

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	AEnemyAIControllerBase* EnemyController = Cast<AEnemyAIControllerBase>(GetController());

	if (EnemyController != nullptr)
	{
		UBlackboardComponent* EnemyBlackboard = EnemyController->GetBlackboardComponent();

		if (EnemyBlackboard != nullptr)
		{
			EnemyBlackboard->SetValueAsObject(FName("PlayerReference"), nullptr);
		}

		EnemyController->NofifySeed();
	}

	if (World != nullptr)
	{
		ABGGameModeBase* GM = Cast<ABGGameModeBase>(World->GetAuthGameMode());

		if (GetLocalRole() == ROLE_Authority)
		{
			if (GM != nullptr)
			{
				GM->ExitCombatState();
			}
		}

		if (DissolveEvent != nullptr)
		{
			FOnAkPostEventCallback NullCallback;
			TArray<FAkExternalSourceInfo> External;
			AkAudioComponent->PostAkEvent(DissolveEvent, int32(0), NullCallback, External, DissolveEvent->GetName());
		}
	}
}

// TODO: Modularize on hit logic by using the on hit delegate
void AEnemyBase::HandleHitResults(TArray<FHitResult> Hits)
{
	for (FHitResult hit : Hits)
	{
		APlayerBase* PlayerToDamage = Cast<APlayerBase>(hit.GetActor());

		if (PlayerToDamage != nullptr)
		{
			PlayerToDamage->SetLastAttackingCharacter(this);
			EHitDirection DiretionHitted = DefineDirectionHitted(this, PlayerToDamage);
			
			if (PlayerToDamage->GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Blocking"))))
			{
				if (DiretionHitted != EHitDirection::FRONT)
				{
					PlayerToDamage->OnDamageExecution();
					HandleDamageExecution(UAbilitySystemBlueprintLibrary::AbilityTargetDataFromHitResult(hit));
				}
				else
				{
					FGameplayEventData EventData;
					FGameplayTag EventTag = FGameplayTag::RequestGameplayTag(FName("Data.Blocked"));
					UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(PlayerToDamage, EventTag, EventData);
				}
			}
			else
			{
				PlayerToDamage->OnDamageExecution();
				HandleDamageExecution(UAbilitySystemBlueprintLibrary::AbilityTargetDataFromHitResult(hit));
			}

			OnHitDel.Broadcast(hit);
		}
	}
}
