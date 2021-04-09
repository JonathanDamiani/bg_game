// Bunny Guard All Rigths.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffect.h"
#include "AbilityActorBase.generated.h"

UCLASS()
class BUNNYGUARD_API AAbilityActorBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAbilityActorBase();

protected:
	FTimerHandle TimeHandle_Lifetime;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Meta = (ExposeOnSpawn = true))
	float ActorLifetime;

	UPROPERTY(BlueprintReadWrite, Meta = (ExposeOnSpawn = true))
	FGameplayEffectSpecHandle DamageEffectSpec;

	UPROPERTY(BlueprintReadWrite, Meta = (ExposeOnSpawn = true))
	FGameplayEffectSpecHandle StunEffectSpec;

	UPROPERTY(BlueprintReadWrite, Meta = (ExposeOnSpawn = true))
	FGameplayEffectSpecHandle SpecialEffectSpec;

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ApplyEffectToTarget(AActor* ActorTarget, FGameplayEffectSpecHandle SpecToApply);
	void ApplyEffectToTarget_Implementation(AActor* ActorTarget, FGameplayEffectSpecHandle SpecToApply);

	UFUNCTION()
	virtual void OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void HandleOnOverlapBeginEvent(AActor* OtherActor);
	virtual void HandleOnOverlapBeginEvent_Implementation(AActor* OtherActor);

	UFUNCTION()
	void StartDamageOverTime();

	UFUNCTION()
	virtual void DestroySelf();
};
