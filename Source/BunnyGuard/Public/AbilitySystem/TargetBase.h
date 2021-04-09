// Bunny Guard All Rigths.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TargetBase.generated.h"

class UStaticMeshComponent;
class ACharacterBase;

UCLASS()
class BUNNYGUARD_API ATargetBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ATargetBase();
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void SetBaseReference(ACharacterBase* PlayerRef);

	UFUNCTION()
	void SetTargetReference(ACharacterBase* TargetRef);

	UFUNCTION()
	void EnableReposition();

	UFUNCTION()
	void DefineActorTransform();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DistanceToSelf;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxDistanceToTarget;

protected:
	virtual void BeginPlay() override;

	ACharacterBase* BaseCharacterReference;

	ACharacterBase* TargetReference;

	bool bEnableReposition = false;
};
