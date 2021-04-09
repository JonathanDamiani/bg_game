// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/AbilityActorBase.h"
#include "ProjectileBase.generated.h"

class UProjectileMovementComponent;
class USphereComponent;
class UNiagaraSystem;

UCLASS()
class BUNNYGUARD_API AProjectileBase : public AAbilityActorBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectileBase();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
	UNiagaraSystem* HitVFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
	float HitSFXScale;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Meta = (ExposeOnSpawn = true))
	int32 NumberOfPiercingEnemies;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Meta = (ExposeOnSpawn = true))
	bool bIsDestroyedOnImpact;

	int32 CurrentPiercingEnemies;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	USphereComponent* ProjectileCollider;

	virtual void HandleOnOverlapBeginEvent_Implementation(AActor* OtherActor) override;
};
