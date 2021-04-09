// Bunny Guard All Rigths.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/ProjectileBase.h"
#include "TornadoBase.generated.h"

class AEnemyBase;

UCLASS()
class BUNNYGUARD_API ATornadoBase : public AProjectileBase
{
	GENERATED_BODY()

public:
	ATornadoBase();

	virtual void HandleOnOverlapBeginEvent_Implementation(AActor* OtherActor) override;

	FTimerHandle TornadoDamage_TimerHandle;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Tornado Data")
	float TornadoDamageRatio;

protected:
	virtual void BeginPlay() override;
};
