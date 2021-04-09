// Bunny Guard All Rigths.


#include "AbilitySystem/AbilityActorBase.h"
#include "Characters/CharacterBase.h"
#include "Characters/Player/PlayerBase.h"
#include "Characters/Enemies/EnemyBase.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

// Sets default values
AAbilityActorBase::AAbilityActorBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	ActorLifetime = 0.0f;

}

// Called when the game starts or when spawned
void AAbilityActorBase::BeginPlay()
{
	Super::BeginPlay();

	OnActorBeginOverlap.AddDynamic(this, &AAbilityActorBase::OnOverlapBegin);

	if (ActorLifetime > 0)
	{
		GetWorldTimerManager().SetTimer(TimeHandle_Lifetime, this, &AAbilityActorBase::DestroySelf, ActorLifetime, false);
	}
}

void AAbilityActorBase::ApplyEffectToTarget_Implementation(AActor* ActorTarget, FGameplayEffectSpecHandle SpecToApply)
{
	ACharacterBase* CharacterToDamage = Cast<ACharacterBase>(ActorTarget);

	if (CharacterToDamage != nullptr)
	{
		CharacterToDamage->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*SpecToApply.Data);
	}
}

void AAbilityActorBase::OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
{

	if (GetInstigator() != nullptr)
	{
		if (GetInstigator() == OtherActor)
		{
			return;
		}
		if (GetInstigator()->GetClass()->IsChildOf(APlayerBase::StaticClass()) && OtherActor->GetClass()->IsChildOf(APlayerBase::StaticClass()))
		{
			return;
		}

		if (GetInstigator()->GetClass()->IsChildOf(AEnemyBase::StaticClass()) && OtherActor->GetClass()->IsChildOf(AEnemyBase::StaticClass()))
		{
			return;
		}

		AEnemyBase* CurrentEnemy = Cast<AEnemyBase>(OtherActor);

		if (CurrentEnemy != nullptr)
		{
			CurrentEnemy->SetLastAttackingCharacter(CurrentEnemy);
		}
	}

	HandleOnOverlapBeginEvent(OtherActor);
}

void AAbilityActorBase::HandleOnOverlapBeginEvent_Implementation(AActor* OtherActor)
{
	ACharacterBase* CharacterToDamage = Cast<ACharacterBase>(OtherActor);

	ACharacterBase* InstigatorChar = Cast<ACharacterBase>(GetInstigator());

	if (CharacterToDamage != nullptr)
	{
		if (DamageEffectSpec != nullptr)
		{
			EHitDirection DiretionHitted = CharacterToDamage->DefineDirectionHitted(this, CharacterToDamage);

			if (CharacterToDamage->GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Blocking"))))
			{
				if (DiretionHitted != EHitDirection::FRONT)
				{
					ApplyEffectToTarget(OtherActor, DamageEffectSpec);
				}
				else
				{
					FGameplayEventData EventData;
					FGameplayTag EventTag = FGameplayTag::RequestGameplayTag(FName("Data.Blocked"));
					UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(CharacterToDamage, EventTag, EventData);
				}
			}
			else
			{
				ApplyEffectToTarget(OtherActor, DamageEffectSpec);
			}
		}

		if (StunEffectSpec != nullptr)
		{
			ApplyEffectToTarget(OtherActor, StunEffectSpec);
		}

		if (SpecialEffectSpec != nullptr)
		{
			ApplyEffectToTarget(OtherActor, SpecialEffectSpec);
		}

		if (InstigatorChar != nullptr)
		{
			InstigatorChar->PlayHitEffect(CharacterToDamage->GetActorLocation());
			CharacterToDamage->PlayHitSFX();
		}
	}
		
}	

void AAbilityActorBase::StartDamageOverTime()
{
	TSet<AActor*> ActorsOverlapped;
	GetOverlappingActors(ActorsOverlapped);

	ACharacterBase* InstigatorChar = Cast<ACharacterBase>(GetInstigator());

	for (AActor* OverlappedActor : ActorsOverlapped)
	{
		AEnemyBase* EnemyToDamage = Cast<AEnemyBase>(OverlappedActor);
		if (EnemyToDamage != nullptr && DamageEffectSpec != nullptr)
		{
			ApplyEffectToTarget(EnemyToDamage, DamageEffectSpec);

			if (InstigatorChar != nullptr)
			{
				InstigatorChar->PlayHitEffect(EnemyToDamage->GetActorLocation());
			}
		}
	}
}

void AAbilityActorBase::DestroySelf()
{
	Destroy();
}
