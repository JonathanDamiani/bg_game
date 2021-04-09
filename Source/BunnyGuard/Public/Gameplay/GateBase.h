// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GateBase.generated.h"


class UStaticMeshComponent;
class UBoxComponent;
class ASeedBase;
class UAkComponent;
class UAkAudioEvent;

UCLASS()
class BUNNYGUARD_API AGateBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGateBase();

	UFUNCTION()
	void OpenGate();

	UFUNCTION(BlueprintNativeEvent)
	void OnDestroy();
	void OnDestroy_Implementation();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Sound)
	UAkComponent* AudioComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* CollisionComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<ASeedBase*> SeedsOfGate;

	virtual void BeginPlay() override;
};
