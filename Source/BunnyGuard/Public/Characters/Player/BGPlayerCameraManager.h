// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "BGPlayerCameraManager.generated.h"

/**
 * 
 */
UCLASS()
class BUNNYGUARD_API ABGPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()
	
public:
	ABGPlayerCameraManager();

	void OnPossess(class APlayerBase* pawn);

	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* CameraBehavior;

	UPROPERTY(EditAnywhere)
	float BackwardsRotationThreshold = -0.85f;

	UPROPERTY(EditAnywhere)
	float POV = 90.0f;

	UPROPERTY(EditAnywhere)
	float ManualToAutoTransitionTime = 2.0f;

	UPROPERTY(EditAnywhere)
	class UAkComponent* AudioComponent;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void DrawDebugTargets(FVector PivotTargetLocation);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FTransform SmoothedPivotTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector PivotLocation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Sound)
	class UAkAudioEvent* MusicEvent = nullptr;

	UFUNCTION(BlueprintCallable)
	float GetCameraBehaviorParam(FName CurveName) const;

	UFUNCTION(BlueprintCallable)
	void SetCameraRotationLag(float Value) const;

protected:
	virtual void UpdateViewTargetInternal(FTViewTarget& OutViewTarget, float DeltaTime) override;

	FVector CalculateAxisIndependentLag(FVector CurrentLocation, FVector TargetLocation, FRotator CameraRotation, FVector LagSpeed, float DeltaTime);

	bool CustomCameraBehavior(float DeltaTime, FVector& OutLocation, FRotator& OutRotation);

	void TraceCameraCollision();

	FRotator CalculateAutomaticCameraRotation(const float& DeltaTime) const;
	
	FRotator CalculateManualCameraRotation(const float& DeltaTime) const;

	FRotator RotateCamera(const FRotator& TargetRotation, const float& DeltaTime, const float& RateOfRotation) const;

	void TurnAutoModeOn();

	void SetAutoCameraSwitchTimer();

private:
	APlayerBase* ControlledCharacter;
	FVector RootLocation;
	FVector TargetCameraLocation;
	FRotator TargetCameraRotation;
	FRotator DebugViewRotation;
	FVector DebugViewOffset;
	bool InAutoMode = true;
	FTimerHandle TimerHandle_ManualMode;
	bool bIsInSpectator;
	int32 MusicPostEventId = -1;
};
