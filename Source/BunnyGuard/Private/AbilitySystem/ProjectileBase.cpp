// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani


#include "AbilitySystem/ProjectileBase.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Characters/CharacterBase.h"
#include "Characters/Player/PlayerBase.h"
#include "Characters/Enemies/EnemyBase.h"
#include "Components/SphereComponent.h"
#include "AbilitySystemComponent.h"
#include "Niagara/Public/NiagaraComponent.h"
#include "Niagara/Public/NiagaraFunctionLibrary.h"

// Sets default values
AProjectileBase::AProjectileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(FName("ProjectileMovement"));

	ProjectileCollider = CreateDefaultSubobject<USphereComponent>(TEXT("ProjectileCollider"));
	ProjectileCollider->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	ProjectileCollider->InitSphereRadius(40.0f);

	NumberOfPiercingEnemies = 0;
	CurrentPiercingEnemies = 0;

	bIsDestroyedOnImpact = true;
}

void AProjectileBase::HandleOnOverlapBeginEvent_Implementation(AActor* OtherActor)
{
	Super::HandleOnOverlapBeginEvent_Implementation(OtherActor);

	UWorld* World = GetWorld();

	if (HitVFX != nullptr && World != nullptr && OtherActor != nullptr)
	{
		FRotator VFXRotation;
		VFXRotation.Yaw = FMath::FRandRange(-180.f, 180.f);;
		VFXRotation.Pitch = FMath::FRandRange(-180.f, 180.f);;
		VFXRotation.Roll = FMath::FRandRange(-180.f, 180.f);;

		FVector SFXScale = FVector(HitSFXScale, HitSFXScale, HitSFXScale);

		UNiagaraComponent* effect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, HitVFX, GetActorLocation(), VFXRotation, SFXScale, true, true, ENCPoolMethod::AutoRelease, true);
	}

	CurrentPiercingEnemies++;

	if (CurrentPiercingEnemies >= NumberOfPiercingEnemies && bIsDestroyedOnImpact)
	{
		Destroy();
	}
}
