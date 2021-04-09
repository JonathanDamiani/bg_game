// Bunny Guard All Rigths.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/AbilityActorBase.h"
#include "LazerBase.generated.h"

/**
 * 
 */
UCLASS()
class BUNNYGUARD_API ALazerBase : public AAbilityActorBase
{
	GENERATED_BODY()
	
	ALazerBase();

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Tornado Data")
	float LazerDamageRatio;

	virtual void HandleOnOverlapBeginEvent_Implementation(AActor* OtherActor) override;

	FTimerHandle LazerDamage_TimerHandle;

	virtual void BeginPlay() override;

	virtual void DestroySelf() override;
};
