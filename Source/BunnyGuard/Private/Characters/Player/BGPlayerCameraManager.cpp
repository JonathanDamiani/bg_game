// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani

#include "Characters/Player/BGPlayerCameraManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Characters/Player/PlayerBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "Characters/Player/Camera/PlayerCameraBehavior.h"
#include "AkGameplayStatics.h"
#include "AkAudioEvent.h"
#include "AkComponent.h"

ABGPlayerCameraManager::ABGPlayerCameraManager()
{
	CameraBehavior = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CameraBehavior"));
	CameraBehavior->SetupAttachment(RootComponent);
	CameraBehavior->bHiddenInGame = true;

	AudioComponent = CreateDefaultSubobject<UAkComponent>(TEXT("AudioComponent "));
	AudioComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	AudioComponent->OcclusionRefreshInterval = 0.0f;

	bIsInSpectator = false;
}

void ABGPlayerCameraManager::OnPossess(APlayerBase* Pawn)
{
	APlayerController* PlayerController = GetOwningPlayerController();

	if (PlayerController == nullptr)
	{
		return;
	}
	PlayerController->SetControlRotation(FRotator::ZeroRotator);
	PlayerController->SetAudioListenerOverride(AudioComponent, FVector::ZeroVector, FRotator::ZeroRotator);

	ControlledCharacter = Pawn;
	UPlayerCameraBehavior* CurrentBehavior = Cast<UPlayerCameraBehavior>(CameraBehavior->GetAnimInstance());

	if (CurrentBehavior != nullptr)
	{
		CurrentBehavior->PlayerController = PlayerController;
		CurrentBehavior->ControlledPawn = ControlledCharacter;
	}

	FOnAkPostEventCallback NullCallback;
	TArray<FAkExternalSourceInfo> External;
	if (MusicEvent != nullptr && MusicPostEventId < 0)
	{
		MusicPostEventId = AudioComponent->PostAkEvent(MusicEvent, int32(0), NullCallback, External, MusicEvent->GetName());
		UAkGameplayStatics::SetState("Global", "Gameplay");
		UAkGameplayStatics::SetState("Gameplay", "Exploration");
	}
}

float ABGPlayerCameraManager::GetCameraBehaviorParam(FName CurveName) const
{
	UAnimInstance* AnimInstance = CameraBehavior->GetAnimInstance();
	if (AnimInstance != nullptr)
	{
		return AnimInstance->GetCurveValue(CurveName);
	}

	return 0.0f;
}

void ABGPlayerCameraManager::SetCameraRotationLag(float Value) const
{
	UAnimInstance* AnimInstance = CameraBehavior->GetAnimInstance();
	UPlayerCameraBehavior* CameraAnimInstance = Cast<UPlayerCameraBehavior>(AnimInstance);
	if (CameraAnimInstance != nullptr)
	{
		CameraAnimInstance->RotationLagSpeed = Value;
	}
}

void ABGPlayerCameraManager::UpdateViewTargetInternal(FTViewTarget & OutViewTarget, float DeltaTime)
{
	if (OutViewTarget.Target == nullptr)
	{
		return;
	}

	FVector OutLocation = FVector::ZeroVector;
	FRotator OutRotation = FRotator::ZeroRotator;

	CustomCameraBehavior(DeltaTime, OutLocation, OutRotation);
	OutViewTarget.POV.Location = OutLocation;
	OutViewTarget.POV.Rotation = OutRotation;
	OutViewTarget.POV.FOV = 90.0f;
}

FVector ABGPlayerCameraManager::CalculateAxisIndependentLag(FVector CurrentLocation, FVector TargetLocation, FRotator CameraRotation, FVector LagSpeed, float DeltaTime)
{
	CameraRotation.Roll = 0.0f;
	CameraRotation.Pitch = 0.0f;
	const FVector UnrotatedCurcLoc = CameraRotation.UnrotateVector(CurrentLocation);
	const FVector UnrotatedTargetLoc = CameraRotation.UnrotateVector(TargetLocation);

	const FVector ResultVector(
		FMath::FInterpTo(UnrotatedCurcLoc.X, UnrotatedTargetLoc.X, DeltaTime, LagSpeed.X),
		FMath::FInterpTo(UnrotatedCurcLoc.Y, UnrotatedTargetLoc.Y, DeltaTime, LagSpeed.Y),
		FMath::FInterpTo(UnrotatedCurcLoc.Z, UnrotatedTargetLoc.Z, DeltaTime, LagSpeed.Z));

	return CameraRotation.RotateVector(ResultVector);
}

bool ABGPlayerCameraManager::CustomCameraBehavior(float DeltaTime, FVector & OutLocation, FRotator & OutRotation)
{
	if (!IsValid(ControlledCharacter) || !ControlledCharacter->IsValidLowLevel())
	{
		return false;
	}
	AudioComponent->SetWorldLocation(ControlledCharacter->GetActorLocation());
	const FTransform& PivotTarget = ControlledCharacter->GetPivotTarget();

	if (ControlledCharacter->HasCameraInput())
	{
		InAutoMode = false;
		SetAutoCameraSwitchTimer();
	}
	TargetCameraRotation = InAutoMode ? CalculateAutomaticCameraRotation(DeltaTime) : CalculateManualCameraRotation(DeltaTime);

	const FVector LagSpeed(GetCameraBehaviorParam(TEXT("PivotLagSpeed_X")), GetCameraBehaviorParam(TEXT("PivotLagSpeed_Y")), GetCameraBehaviorParam(TEXT("PivotLagSpeed_Z")));
	const FVector AxisIndependentLag = CalculateAxisIndependentLag(SmoothedPivotTarget.GetLocation(), PivotTarget.GetLocation(), TargetCameraRotation, LagSpeed, DeltaTime);

	SmoothedPivotTarget.SetRotation(PivotTarget.GetRotation());
	SmoothedPivotTarget.SetLocation(AxisIndependentLag);
	SmoothedPivotTarget.SetScale3D(FVector::OneVector);

	PivotLocation = SmoothedPivotTarget.GetLocation() +
		UKismetMathLibrary::GetForwardVector(SmoothedPivotTarget.Rotator()) * GetCameraBehaviorParam(TEXT("PivotOffset_X")) +
		UKismetMathLibrary::GetRightVector(SmoothedPivotTarget.Rotator()) * GetCameraBehaviorParam(TEXT("PivotOffset_Y")) +
		UKismetMathLibrary::GetUpVector(SmoothedPivotTarget.Rotator()) * GetCameraBehaviorParam(TEXT("PivotOffset_Z"));

	float OffsetX = GetCameraBehaviorParam(TEXT("CameraOffset_X"));

	TargetCameraLocation = UKismetMathLibrary::VLerp(
		PivotLocation +
		UKismetMathLibrary::GetForwardVector(TargetCameraRotation) * GetCameraBehaviorParam(TEXT("CameraOffset_X")) +
		UKismetMathLibrary::GetRightVector(TargetCameraRotation) * GetCameraBehaviorParam(TEXT("CameraOffset_Y")) +
		UKismetMathLibrary::GetUpVector(TargetCameraRotation) * GetCameraBehaviorParam(TEXT("CameraOffset_Z")),
		PivotTarget.GetLocation(),
		0.0f
	);

	TraceCameraCollision();
	DrawDebugTargets(PivotTarget.GetLocation());

	OutLocation = TargetCameraLocation;
	OutRotation = TargetCameraRotation;

	return true;
}

void ABGPlayerCameraManager::TraceCameraCollision()
{
	FVector TraceOrigin;
	float TraceRadius = 0.0f;
	ECollisionChannel TraceChannel = ControlledCharacter->GetCameraTraceParams(TraceOrigin, TraceRadius);

	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(ControlledCharacter);

	FHitResult Result;
	World->SweepSingleByChannel(Result, TraceOrigin, TargetCameraLocation, FQuat::Identity, TraceChannel, FCollisionShape::MakeSphere(TraceRadius), Params);

	if (Result.IsValidBlockingHit())
	{
		TargetCameraLocation += (Result.Location - Result.TraceEnd);
	}
}

FRotator ABGPlayerCameraManager::CalculateAutomaticCameraRotation(const float& DeltaTime) const
{
	float DotProduct = FVector::DotProduct(GetActorForwardVector(), ControlledCharacter->GetActorForwardVector());
	if (DotProduct < BackwardsRotationThreshold)
	{
		return TargetCameraRotation;
	}

	return RotateCamera(ControlledCharacter->GetActorRotation(), DeltaTime, GetCameraBehaviorParam(TEXT("AutoRotationLagSpeed")));
}

FRotator ABGPlayerCameraManager::CalculateManualCameraRotation(const float& DeltaTime) const
{
	return RotateCamera(GetOwningPlayerController()->GetControlRotation(), DeltaTime, GetCameraBehaviorParam(TEXT("RotationLagSpeed")));
}

FRotator ABGPlayerCameraManager::RotateCamera(const FRotator & TargetRotation, const float& DeltaTime, const float & RateOfRotation) const
{
	FRotator CurrentRotation = FMath::RInterpTo(GetCameraRotation(), TargetRotation, DeltaTime, RateOfRotation);
	GetOwningPlayerController()->SetControlRotation(CurrentRotation);
	return CurrentRotation;
}

void ABGPlayerCameraManager::TurnAutoModeOn()
{
	InAutoMode = true;
}

void ABGPlayerCameraManager::SetAutoCameraSwitchTimer()
{
	const UWorld* const World = GetWorld();

	if (World != nullptr)
	{
		World->GetTimerManager().SetTimer(TimerHandle_ManualMode, this, &ABGPlayerCameraManager::TurnAutoModeOn, ManualToAutoTransitionTime, false);
	}
}
