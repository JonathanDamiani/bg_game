// Bunny Guard All Rigths.


#include "AbilitySystem/VortexBase.h"
#include "Characters/Enemies/EnemyBase.h"

void AVortexBase::HandleOnOverlapBeginEvent_Implementation(AActor* OtherActor)
{
	AEnemyBase* EnemyToApplyBehaviour = Cast<AEnemyBase>(OtherActor);

	if (EnemyToApplyBehaviour != nullptr)
	{
		EnemyToApplyBehaviour->SetVortexReference(this);
	}

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

		if (DamageEffectSpec != nullptr)
		{
			ApplyEffectToTarget(OtherActor, DamageEffectSpec);
		}
	}
}

AVortexBase::AVortexBase()
{

}

void AVortexBase::BeginPlay()
{
	Super::BeginPlay();
}
