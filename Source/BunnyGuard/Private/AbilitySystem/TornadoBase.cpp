// Bunny Guard All Rigths.


#include "AbilitySystem/TornadoBase.h"
#include "Characters/Enemies/EnemyBase.h"

ATornadoBase::ATornadoBase()
{
	TornadoDamageRatio = 0.5f;
}

void ATornadoBase::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(TornadoDamage_TimerHandle, this, &ATornadoBase::StartDamageOverTime, TornadoDamageRatio, true);
}

void ATornadoBase::HandleOnOverlapBeginEvent_Implementation(AActor* OtherActor)
{

	AEnemyBase* EnemyToApplyBehaviour = Cast<AEnemyBase>(OtherActor);

	if (EnemyToApplyBehaviour != nullptr)
	{
		EnemyToApplyBehaviour->SetTornadoReference(this);
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
	}
}
