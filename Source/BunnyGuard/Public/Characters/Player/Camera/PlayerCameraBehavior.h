// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerCameraBehavior.generated.h"

/**
 * 
 */
UCLASS()
class BUNNYGUARD_API UPlayerCameraBehavior : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class APlayerBase* ControlledPawn;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class APlayerController* PlayerController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float RotationLagSpeed = 20.0f;
};
