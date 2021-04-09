// Bunny Guard All Rigths.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/AbilityActorBase.h"
#include "VortexBase.generated.h"

/**
 * 
 */
UCLASS()
class BUNNYGUARD_API AVortexBase : public AAbilityActorBase
{
	GENERATED_BODY()

public:
	AVortexBase();

	virtual void HandleOnOverlapBeginEvent_Implementation(AActor* OtherActor) override;

protected:
	virtual void BeginPlay() override;
};
