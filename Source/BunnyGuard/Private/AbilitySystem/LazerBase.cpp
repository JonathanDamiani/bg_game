// Bunny Guard All Rigths.


#include "AbilitySystem/LazerBase.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Characters/CharacterBase.h"

ALazerBase::ALazerBase()
{
	LazerDamageRatio = 0.5;
}

void ALazerBase::HandleOnOverlapBeginEvent_Implementation(AActor* OtherActor)
{
	ACharacterBase* CharacterToDamage = Cast<ACharacterBase>(OtherActor);

	if (CharacterToDamage != nullptr)
	{
		if (StunEffectSpec != nullptr)
		{
			ApplyEffectToTarget(OtherActor, StunEffectSpec);
		}

		if (SpecialEffectSpec != nullptr)
		{
			ApplyEffectToTarget(OtherActor, SpecialEffectSpec);
		}
	}
}

void ALazerBase::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(LazerDamage_TimerHandle, this, &ALazerBase::StartDamageOverTime, LazerDamageRatio, true);
}

void ALazerBase::DestroySelf()
{
	FGameplayEventData EventData;

	FGameplayTag EventTag = FGameplayTag::RequestGameplayTag(FName("Event.Finish"));
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetInstigator(), EventTag, EventData);

	Super::DestroySelf();
}
