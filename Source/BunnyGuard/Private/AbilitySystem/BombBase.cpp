// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani


#include "AbilitySystem/BombBase.h"
#include "Characters/CharacterBase.h"
#include "Characters/Player/PlayerBase.h"
#include "Characters/Enemies/EnemyBase.h"
#include "Components/SphereComponent.h"
#include "AbilitySystemComponent.h"

// Sets default values
ABombBase::ABombBase()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

	BombCollider = CreateDefaultSubobject<USphereComponent>(TEXT("BombCollider"));
	BombCollider->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	BombCollider->InitSphereRadius(200.0f);

	BombCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	BombLifetime = 2.0f;
}

void ABombBase::OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
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
	}

	ACharacterBase* CharacterToDamage = Cast<ACharacterBase>(OtherActor);

	if (CharacterToDamage != nullptr)
	{
		ApplyBombEffect(CharacterToDamage);
	}
}

// Called when the game starts or when spawned
void ABombBase::BeginPlay()
{
	Super::BeginPlay();

	OnActorBeginOverlap.AddDynamic(this, &ABombBase::OnOverlapBegin);

	GetWorldTimerManager().SetTimer(TimeHandle_BombLifetime, this, &ABombBase::ExplodeBomb, BombLifetime, false);
}

void ABombBase::ApplyBombEffect_Implementation(AActor* ActorToApply)
{
	ACharacterBase* CharacterToApplyEffect = Cast<ACharacterBase>(ActorToApply);

	if (CharacterToApplyEffect != nullptr)
	{
		CharacterToApplyEffect->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*DamageEffectSpec.Data);
		CharacterToApplyEffect->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*StunEffectSpec.Data);
	}
}

void ABombBase::ExplodeBomb()
{
	BombCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Destroy();
}
