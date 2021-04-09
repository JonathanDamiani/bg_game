// Copyright (C) 2020 Alejandro G�ereca Valdivia / Jonathan Dean Damiani


#include "Characters/CharacterBase.h"
#include "AbilitySystem/BGAbilitySystemComponent.h"
#include "AbilitySystem/AttributeSetBase.h"
#include "AbilitySystem/GameplayAbilityBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Characters/Player/PlayerBase.h"
#include "BGGameInstance.h"
#include "Characters/Player/PlayerStateBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "AkComponent.h"
#include "AkGameplayStatics.h"
#include "AkAudioEvent.h"
#include "Niagara/Public/NiagaraComponent.h"
#include "Niagara/Public/NiagaraFunctionLibrary.h"

// Sets default values
ACharacterBase::ACharacterBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	AkAudioComponent = CreateDefaultSubobject<UAkComponent>(TEXT("AkAudioComponent"));
	AkAudioComponent->SetupAttachment(GetMesh());
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Overlap);

	bAlwaysRelevant = true;

	NetUpdateFrequency = 50.0f;

	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
	BlockMoveTag = FGameplayTag::RequestGameplayTag(FName("State.BlockMove"));
	EffectRemoveOnDeathTag = FGameplayTag::RequestGameplayTag(FName("Effect.RemoveOnDeath"));

	CollisionChannelToDamage = ECC_GameTraceChannel1;

	InKnockBack = false;

	HitSFXScale = 1.0f;
}

UAbilitySystemComponent* ACharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent.Get();
}

float ACharacterBase::GetHealth() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetHealth();
	}

	return 0.0f;
}

float ACharacterBase::GetMaxHealth() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetMaxHealth();
	}

	return 0.0f;
}

float ACharacterBase::GetMoveSpeed() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetMoveSpeed();
	}

	return 0.0f;
}

bool ACharacterBase::IsAlive() const
{
	return GetHealth() > 0.0f;
}

void ACharacterBase::Die()
{
	if (FaintEvent != nullptr)
	{
		FOnAkPostEventCallback NullCallback;
		TArray<FAkExternalSourceInfo> External;
		AkAudioComponent->PostAkEvent(FaintEvent, int32(0), NullCallback, External, FaintEvent->GetName());
	}

	OnCharacterDie.Broadcast(this);

	if (AbilitySystemComponent.IsValid())
	{
		AbilitySystemComponent->CancelAllAbilities();
		AbilitySystemComponent->AddLooseGameplayTag(DeadTag);
	}

	AkAudioComponent->Stop();
	UAkGameplayStatics::StopActor(this);
}

void ACharacterBase::AddCharacterAbilities()
{
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent.IsValid() || AbilitySystemComponent->bCharacterAbilitiesGiven)
	{
		return;
	}

	for (TSubclassOf<UGameplayAbilityBase>& StartupAbility : StartAbilities)
	{
		AbilitySystemComponent->GiveAbility(
			FGameplayAbilitySpec(StartupAbility, 1, static_cast<int32>(StartupAbility.GetDefaultObject()->AbilityInputID), this));
	}

	AbilitySystemComponent->bCharacterAbilitiesGiven = true;
}

void ACharacterBase::RemoveCharacterAbilities()
{
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent.IsValid() || !AbilitySystemComponent->bCharacterAbilitiesGiven)
	{
		return;
	}

	TArray<FGameplayAbilitySpecHandle> AbilitiesToRemove;
	for (const FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities())
	{
		if ((Spec.SourceObject == this) && StartAbilities.Contains(Spec.Ability->GetClass()))
		{
			AbilitiesToRemove.Add(Spec.Handle);
		}
	}

	for (int32 i = 0; i < AbilitiesToRemove.Num(); i++)
	{
		AbilitySystemComponent->ClearAbility(AbilitiesToRemove[i]);
	}

	AbilitySystemComponent->bCharacterAbilitiesGiven = false;
}

void ACharacterBase::InitializeAttributes()
{
	if (!AbilitySystemComponent.IsValid())
	{
		return;
	}

	if (!DefaultAttributes)
	{
		return;
	}

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributes, 1, EffectContext);
	if (NewHandle.IsValid())
	{
		FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent.Get());
	}
}

void ACharacterBase::AddStartupEffects()
{
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent.IsValid() || AbilitySystemComponent->bStartupEffectsApplied)
	{
		return;
	}

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	for (TSubclassOf<UGameplayEffect> GameplayEffect : StartupEffects)
	{
		FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffect, 1, EffectContext);
		if (NewHandle.IsValid())
		{
			FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent.Get());
		}
	}

	AbilitySystemComponent->bStartupEffectsApplied = true;
}

void ACharacterBase::HandleDamageExecution_Implementation(FGameplayAbilityTargetDataHandle TargeToDamage)
{
	FGameplayEventData EventData;
	EventData.TargetData = TargeToDamage;

	FGameplayTag EventTag = FGameplayTag::RequestGameplayTag(FName("Data.Damage"));
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, EventTag, EventData);
}

void ACharacterBase::SetHealth(float Health)
{
	if (AttributeSetBase.IsValid())
	{
		AttributeSetBase->SetHealth(Health);
	}
}


//////////////////////////////////////////////////////////////////////////
// Ability handles

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();

	bool AudioComponentExist = AkAudioComponent != nullptr;
	AkAudioComponent = UAkGameplayStatics::GetAkComponent(GetMesh(), AudioComponentExist, FName("SoundSocket"));
	AkAudioComponent->SetSwitch(FString("FS_Shoe_Type"), FootstepState);
	AkAudioComponent->SetSwitch(FString("Character_Select"), CharacterState);
	AkAudioComponent->OcclusionRefreshInterval = 0.0f;
	
	OnHitDel.AddDynamic(this, &ACharacterBase::OnHitEvent);
}

void ACharacterBase::ActiveAbilityHandle(EAbilityType AbilityTypeEnum)
{
	switch (AbilityTypeEnum)
	{
	case EAbilityType::BASIC_ATTACK:
		ExecuteBasicAttack();
		break;

	case EAbilityType::CHARGED_ATTACK:
		ExecuteChargedAttack();
		break;

	case EAbilityType::UTILITY:
		ExecuteUtility();
		break;

	case EAbilityType::COMBAT:
		ExecuteCombat();
		break;

	case EAbilityType::ULTIMATE:
		ExecuteUltimate();
		break;

	default:
		break;
	}
}

void ACharacterBase::ExecuteBasicAttack_Implementation()
{	
	// Implemented in child
}

void ACharacterBase::ExecuteChargedAttack_Implementation()
{
	// Implemented in child
}

void ACharacterBase::ExecuteUtility_Implementation()
{
	// Implemented in child
}

void ACharacterBase::ExecuteCombat_Implementation()
{
	// Implemented in child
}

void ACharacterBase::ExecuteUltimate_Implementation()
{
	// Implemented in child
}

EHitDirection ACharacterBase::DefineDirectionHitted(AActor* CharacterInstigator, AActor* CharacterDamaged)
{	
	FVector CharacterDamagedForward = CharacterDamaged->GetActorForwardVector();
	
	FVector DamagedCharacterToInstigator = CharacterInstigator->GetActorLocation() - CharacterDamaged->GetActorLocation();
	
	DamagedCharacterToInstigator.Normalize();

	float DotFromVectors = FVector::DotProduct(CharacterDamagedForward, DamagedCharacterToInstigator);

	CharacterDamagedForward.Z = 0.0f;
	DamagedCharacterToInstigator.Z = 0.0f;

	FVector CrossProd = FVector::CrossProduct(CharacterDamagedForward, DamagedCharacterToInstigator);

	float VisionAngleFront = FMath::RadiansToDegrees(FMath::Acos(DotFromVectors));
	
	float VisionAngleRight = CrossProd.Z;

	if (VisionAngleFront > 0 && VisionAngleFront <= 70.0f)
	{
		return EHitDirection::FRONT;
	}

	if (VisionAngleFront > 120 && VisionAngleFront <= 180.0f)
	{
		return EHitDirection::BACK;
	}
	if (VisionAngleRight > 0.0f)
	{
		return EHitDirection::RIGHT;
	}
	if (VisionAngleRight < 0.0f)
	{
		return EHitDirection::LEFT;
	}

	return EHitDirection::FRONT;
}

void ACharacterBase::OnHitEvent_Implementation(FHitResult CharacterHitResult)
{
	ACharacterBase* HittedCharacter = Cast<ACharacterBase>(CharacterHitResult.GetActor());

	if (IsValid(HittedCharacter) == false)
	{
		return;
	}

	PlayHitEffect(CharacterHitResult.ImpactPoint);
	HittedCharacter->PlayHitSFX();
}

ACharacterBase* ACharacterBase::GetLastAttackingCharacter() const
{
	return LastAttackingCharacter;
}

void ACharacterBase::SetLastAttackingCharacter(ACharacterBase* AttackingActor)
{
	LastAttackingCharacter = AttackingActor;
}

void ACharacterBase::PlayHitEffect(FVector position)
{
	UWorld* World = GetWorld();

	if (HitVFX != nullptr && World != nullptr)
	{
		FRotator VFXRotation;
		VFXRotation.Yaw = FMath::FRandRange(-180.f, 180.f);;
		VFXRotation.Pitch = FMath::FRandRange(-180.f, 180.f);;
		VFXRotation.Roll = FMath::FRandRange(-180.f, 180.f);;

		FVector SFXScale = FVector(HitSFXScale, HitSFXScale, HitSFXScale);

		UNiagaraComponent* effect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, HitVFX, position, VFXRotation, SFXScale, true, true, ENCPoolMethod::AutoRelease, true);
	}
	
}

void ACharacterBase::SetHitVFX(UNiagaraSystem* HitVFXToChange, float HitVFXSizeScale)
{
	if (HitVFXToChange != nullptr)
	{
		HitVFX = HitVFXToChange;
		HitSFXScale = HitVFXSizeScale;
	}
}

FString ACharacterBase::GetSFXCharacterState() const
{
	return CharacterState;
}

void ACharacterBase::PlayHitSFX()
{
	FOnAkPostEventCallback NullCallback;
	TArray<FAkExternalSourceInfo> External;
	
	if (GetHitEvent != nullptr)
	{
		AkAudioComponent->PostAkEvent(GetHitEvent, int32(0), NullCallback, External, GetHitEvent->GetName());
	}
}

void ACharacterBase::HandleReactionBehaviour_Implementation(EHitDirection EDirectionHitted, ACharacterBase* CharacterToReact)
{
	
	switch (EDirectionHitted)
	{
	case EHitDirection::RIGHT:
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Right"));
		break;
	case EHitDirection::LEFT:
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Left"));
		break;
	case EHitDirection::FRONT:
		//	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Front"));
		break;
	case EHitDirection::BACK:
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Back"));
		break;
	default:
		break;
	}
}

void ACharacterBase::SphereAttack(float StartMultiplier, float EndMultiplier, float RadiusOfAttack, bool EnableDebug)
{
	UWorld* World = GetWorld();

	if (World == nullptr)
	{
		return;
	}

	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	TArray<FHitResult> OutHits;

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);

	FVector StartLocation = (GetActorForwardVector() * StartMultiplier) + GetActorLocation();
	FVector EndLocation = (GetActorForwardVector() * EndMultiplier) + GetActorLocation();

	if (EnableDebug)
	{
		UKismetSystemLibrary::SphereTraceMulti(World, StartLocation, EndLocation, RadiusOfAttack, UEngineTypes::ConvertToTraceType(CollisionChannelToDamage), false, IgnoreActors, EDrawDebugTrace::ForDuration, OutHits, true);
	}
	else
	{
		UKismetSystemLibrary::SphereTraceMulti(World, StartLocation, EndLocation, RadiusOfAttack, UEngineTypes::ConvertToTraceType(CollisionChannelToDamage), false, IgnoreActors, EDrawDebugTrace::None, OutHits, true);
	}

	HandleHitResults(OutHits);
}

void ACharacterBase::HandleHitResults(TArray<FHitResult> Hits)
{
	for (FHitResult hit : Hits)
	{
		ACharacterBase* CharacterDamaged = Cast<ACharacterBase>(hit.GetActor());

		if (CharacterDamaged != nullptr)
		{
			EHitDirection DirectionHitted = DefineDirectionHitted(this, CharacterDamaged);

			CharacterDamaged->SetLastAttackingCharacter(this);

			OnHitDel.Broadcast(hit);
		}

		HandleDamageExecution(UAbilitySystemBlueprintLibrary::AbilityTargetDataFromHitResult(hit));
	}
}

void ACharacterBase::HandleKnockbackReaction_Implementation(ACharacterBase* CharacterThatHittedMe)
{
	if (CharacterThatHittedMe != nullptr)
	{
		FVector AttackerPosition = CharacterThatHittedMe->GetActorLocation();
		FRotator LookDirection = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), AttackerPosition);
		FRotator KnockbackRotation(0.0f, LookDirection.Yaw, LookDirection.Roll);

		SetActorRotation(KnockbackRotation);
	}
}
