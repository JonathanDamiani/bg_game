// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CheckpointActor.generated.h"

class UBoxComponent;

UCLASS()
class BUNNYGUARD_API ACheckpointActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACheckpointActor();

protected:

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UBoxComponent* BoxCollider;
};
