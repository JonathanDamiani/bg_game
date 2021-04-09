// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffect.h"
#include "BombBase.generated.h"

class USphereComponent;

UCLASS()
class BUNNYGUARD_API ABombBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABombBase();

	UFUNCTION()
	void OnOverlapBegin(AActor* OverlappedActor, AActor* ActorToApply);

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Meta = (ExposeOnSpawn = true))
	float BombLifetime;

	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	USphereComponent* BombCollider;
	
	UPROPERTY(BlueprintReadWrite, Meta = (ExposeOnSpawn = true))
	FGameplayEffectSpecHandle DamageEffectSpec;

	UPROPERTY(BlueprintReadWrite, Meta = (ExposeOnSpawn = true))
	FGameplayEffectSpecHandle StunEffectSpec;

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ApplyBombEffect(AActor* ActorToDamage);
	void ApplyBombEffect_Implementation(AActor* ActorToDamage);

	FTimerHandle TimeHandle_BombLifetime;

	void ExplodeBomb();
};
