// Copyright (C) 2020 Alejandro G�ereca Valdivia / Jonathan Dean Damiani

#include "Characters/Player/PlayerBase.h"
#include "Engine.h"
#include "Engine/Blueprint.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "BunnyGuard/BunnyGuard.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Characters/Player/PlayerControllerBase.h"
#include "Characters/Player/PlayerStateBase.h"
#include "AbilitySystem/BGAbilitySystemComponent.h"
#include "AbilitySystem/AttributeSetBase.h"
#include "AbilitySystem/TargetBase.h"
#include "BGGameModeBase.h"
#include "Components/TextRenderComponent.h"
#include "Gameplay/CheckpointActor.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Characters/Enemies/EnemyBase.h"
#include "Characters/Player/PlayerControllerBase.h"
#include "Characters/BGCharacterMovementComponent.h"
#include "UI/PlayerHUDBase.h"
#include "AkComponent.h"
#include "AkAudioEvent.h"
#include "AkGameplayStatics.h"
#include "BunnyGuardHttpRequestHandler.h"
#include "Components/InputComponent.h"
#include "BGGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimMontage.h"
#include "Characters/Player/BGPlayerCameraManager.h"

APlayerBase::APlayerBase(const class FObjectInitializer& ObjectInitializer) : 
	Super(ObjectInitializer.SetDefaultSubobjectClass<UBGCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel3, ECollisionResponse::ECR_Overlap);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Overlap);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; 
	CameraBoom->bUsePawnControlRotation = true; 

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false; 

	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionProfileName(FName("NoCollision"));

	InteractCollider = CreateDefaultSubobject<USphereComponent>(TEXT("InteractCollider"));
	InteractCollider->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	InteractCollider->InitSphereRadius(115.0f);
	FVector InteractColliderLocation = FVector(80.0f, 0.0f, 0.0f);

	InteractCollider->SetRelativeLocation(InteractColliderLocation);

	CombatCheckCollider = CreateDefaultSubobject<USphereComponent>(TEXT("CombatCheckCollider"));
	CombatCheckCollider->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	CombatCheckCollider->InitSphereRadius(500.0f);
	CombatCheckCollider->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Ignore);
	FVector CombatCheckColliderLocation = FVector(0.0f, 0.0f, 0.0f);

	CombatCheckCollider->SetRelativeLocation(CombatCheckColliderLocation);

	TextReviveComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextComponent"));
	TextReviveComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
	DisableAbilitiesTag = FGameplayTag::RequestGameplayTag(FName("State.DisableAbilities"));
	bCanMove = true;

	InteractableActor = nullptr;

	PrimaryActorTick.bCanEverTick = true;
}

void APlayerBase::ActiveAbilityHandle(EAbilityType AbilityTypeEnum)
{
	Super::ActiveAbilityHandle(AbilityTypeEnum);

	APlayerStateBase* PS = GetPlayerState<APlayerStateBase>();

	if (PS != nullptr && TelemetryId > 0)
	{
		BunnyGuardHttpRequestHandler::NotifyAbilityUse(TelemetryId, AbilityTypeEnum);
	}
}

void APlayerBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerBase::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerBase::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APlayerBase::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APlayerBase::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Interact", EInputEvent::IE_Pressed, this, &APlayerBase::HandleInteractions);

	BindASCInput();
}

void APlayerBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	APlayerStateBase* PS = GetPlayerState<APlayerStateBase>();

	if (PS)
	{
		AbilitySystemComponent = Cast<UBGAbilitySystemComponent>(PS->GetAbilitySystemComponent());

		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);

		AttributeSetBase = PS->GetAttributeSet();

		InitializeAttributes();

		AddStartupEffects();

		AddCharacterAbilities();

		APlayerControllerBase* PC = Cast<APlayerControllerBase>(GetController());
		if (PC)
		{
			PC->CreatePlayerHUD();

			PC->AttachCamera();
		}

		AbilitySystemComponent->SetTagMapCount(DeadTag, 0);

		SetHealth(GetMaxHealth());
	}

}

void APlayerBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	APlayerStateBase* PS = GetPlayerState<APlayerStateBase>();

	if (PS)
	{
		AbilitySystemComponent = Cast<UBGAbilitySystemComponent>(PS->GetAbilitySystemComponent());

		AbilitySystemComponent->InitAbilityActorInfo(PS, this);

		BindASCInput();

		AttributeSetBase = PS->GetAttributeSet();

		InitializeAttributes();

		AddCharacterAbilities();

		APlayerControllerBase* PC = Cast<APlayerControllerBase>(GetController());
		
		if (PC)
		{
			PC->CreatePlayerHUD();

		}

		AbilitySystemComponent->SetTagMapCount(DeadTag, 0);

		SetHealth(GetMaxHealth());
	}

	UpdatePlayersInfoServer();
}

void APlayerBase::UpdatePlayersInfoServer_Implementation()
{
	UWorld* World = GetWorld();

	if (World != nullptr)
	{
		ABGGameState* GS = Cast<ABGGameState>(UGameplayStatics::GetGameState(World));

		if (GS != nullptr)
		{
			GS->SetAllPlayersHUDInfo();
		}
	}
}

EPlayerType APlayerBase::GetPlayerType() const
{
	return PlayerType;
}

void APlayerBase::PlayChargeReleaseSFX_Implementation()
{
	if (ChargeReleaseEvent != nullptr)
	{
		FOnAkPostEventCallback NullCallback;
		TArray<FAkExternalSourceInfo> External;
		AkAudioComponent->PostAkEvent(ChargeReleaseEvent, int32(0), NullCallback, External, ChargeReleaseEvent->GetName());
	}
}

void APlayerBase::BeginPlay()
{
	Super::BeginPlay();

	APlayerControllerBase* PlayerController = Cast<APlayerControllerBase>(GetController());
	if (PlayerController == nullptr)
	{
		return;
	}

	PlayerController->AttachCamera();

	InteractCollider->OnComponentBeginOverlap.AddDynamic(this, &APlayerBase::OnOverlapBegin);
	InteractCollider->OnComponentEndOverlap.AddDynamic(this, &APlayerBase::OnOverlapEnd);

	CombatCheckCollider->OnComponentBeginOverlap.AddDynamic(this, &APlayerBase::OnCombatBeginOverlap);
	CombatCheckCollider->OnComponentEndOverlap.AddDynamic(this, &APlayerBase::OnCombatEndOverlap);
}

//////////////////////////////////////////////////////////////////////////
// Player Movement 
//////////////////////////////////////////////////////////////////////////

void APlayerBase::MoveForward(float Value)
{
	if (IsAlive() && bCanMove && !FMath::IsNearlyEqual(Value, 0.0f))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
		SetPlayerRotationClient(GetLastMovementInputVector());

		if (GetCharacterMovement()->bOrientRotationToMovement == false && RotationMode != ERotationMode::Aiming)
		{
			RotatePlayerClient(this);
		}
	}
}

void APlayerBase::MoveRight(float Value)
{
	if (IsAlive() && bCanMove && !FMath::IsNearlyEqual(Value, 0.0f))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
		SetPlayerRotationClient(GetLastMovementInputVector());

		if (GetCharacterMovement()->bOrientRotationToMovement == false && RotationMode != ERotationMode::Aiming)
		{
			RotatePlayerClient(this);
		}
	}
}


//////////////////////////////////////////////////////////////////////////
// Die
//////////////////////////////////////////////////////////////////////////

void APlayerBase::Die()
{
	Super::Die();

	if (DeathMontage != nullptr)
	{
		PlayAnimMontage(DeathMontage);
	}

	UWorld* World = GetWorld();

	UAkGameplayStatics::SetState("Is_Injured", "True");

	if (World != nullptr)
	{
		ABGGameModeBase* GM = Cast<ABGGameModeBase>(World->GetAuthGameMode());

		if (GetLocalRole() == ROLE_Authority)
		{
			if (GM != nullptr)
			{
				GM->CheckDefeatCondition();
			}
		}
	}
}


//////////////////////////////////////////////////////////////////////////
// Arrows to other players 
//////////////////////////////////////////////////////////////////////////

void APlayerBase::FindOtherAndPointToThem_Implementation()
{
	if (ArrowActorToOtherPlayer == nullptr)
	{
		return;
	}

	UWorld* World = GetWorld();

	if (ArrowsToOtherPlayers.Num() > 0)
	{
		for (ATargetBase* TargetArrow : ArrowsToOtherPlayers)
		{
			if (TargetArrow != nullptr)
			{
				TargetArrow->Destroy();
			}
		}
	}

	ArrowsToOtherPlayers.Empty();

	if (World != nullptr)
	{
		ABGGameState* GS = Cast<ABGGameState>(UGameplayStatics::GetGameState(World));

		if (GS != nullptr)
		{
			for (APlayerState* PS : GS->PlayerArray)
			{
				if ( PS != nullptr && PS != GetPlayerState())
				{
					ACharacterBase* OtherPlayer = Cast<ACharacterBase>(PS->GetPawn());

					if (OtherPlayer != nullptr)
					{
						FActorSpawnParameters SpawnParameters;
						SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

						ATargetBase* TargetActor = World->SpawnActor<ATargetBase>(ArrowActorToOtherPlayer, GetActorLocation(), GetActorRotation(), SpawnParameters);
						ArrowsToOtherPlayers.Add(TargetActor);
						TargetActor->SetTargetReference(this);
						TargetActor->SetBaseReference(OtherPlayer);
						TargetActor->EnableReposition();
					}
				}
			}
		}
	}

}

void APlayerBase::Revive_Implementation()
{
	TextReviveComponent->SetText("");
	SetHealth(GetMaxHealth());
	AbilitySystemComponent->RemoveLooseGameplayTag(DeadTag);
	AbilitySystemComponent->SetTagMapCount(DeadTag, 0);

	if (ReviveMontage != nullptr)
	{
		PlayAnimMontage(ReviveMontage);
	}

	if (ReviveEvent != nullptr)
	{
		FOnAkPostEventCallback NullCallback;
		TArray<FAkExternalSourceInfo> External;
		AkAudioComponent->PostAkEvent(ReviveEvent, int32(0), NullCallback, External, ReviveEvent->GetName());
	}

	UAkGameplayStatics::SetState("Is_Injured", "False");
}

void APlayerBase::SetPlayerCanMove(bool bIsPlayerCanMove)
{
	bCanMove = bIsPlayerCanMove;
}

void APlayerBase::DisableAbilities()
{
	if (AbilitySystemComponent.IsValid())
	{
		AbilitySystemComponent->AddLooseGameplayTag(DisableAbilitiesTag);
	}
}

void APlayerBase::EnableAbilities()
{
	if (AbilitySystemComponent.IsValid())
	{
		AbilitySystemComponent->RemoveLooseGameplayTag(DisableAbilitiesTag);
		AbilitySystemComponent->SetTagMapCount(DisableAbilitiesTag, 0);
	}
}

void APlayerBase::ShowEndGameScreen_Implementation(bool bIsVictory)
{
	APlayerControllerBase* PC = Cast<APlayerControllerBase>(GetController());

	if (PC != nullptr && PC->IsLocalPlayerController())
	{
		PC->CreateEndGameScreen(bIsVictory);
	}
}

void APlayerBase::HandleInteractions_Implementation()
{
	if (InteractableActor != nullptr)
	{
		IInteractionInterface* InteractInterface = Cast<IInteractionInterface>(InteractableActor);

		if (InteractInterface != nullptr)
		{
			InteractInterface->Execute_OnInteract(InteractableActor, this);
		}
	}
}

void APlayerBase::HandleCheckpoint_Implementation()
{
	if (CheckpointReference == nullptr)
	{
		return;
	}

	FVector LocationToTeleport = CheckpointReference->GetActorLocation();
	FRotator RotationToTeleport = CheckpointReference->GetActorRotation();

	TeleportTo(LocationToTeleport, RotationToTeleport);
}

void APlayerBase::HandleAimPressed()
{
	RotationMode = ERotationMode::Aiming;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	bUseControllerRotationYaw = true; 

	APlayerControllerBase* PC = Cast<APlayerControllerBase>(GetController());

	if (PC)
	{
		UPlayerHUDBase* PHUD = PC->GetPlayerHUD();

		if (PHUD != nullptr)
		{
			PHUD->SetVisibilityOfCrosshair(true);
		}
	}
}

void APlayerBase::HandleAimReleased()
{
	RotationMode = ERotationMode::Velocity;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;

	APlayerControllerBase* PC = Cast<APlayerControllerBase>(GetController());

	if (PC)
	{
		UPlayerHUDBase* PHUD = PC->GetPlayerHUD();

		if (PHUD != nullptr)
		{
			PHUD->SetVisibilityOfCrosshair(false);
		}
	}
	
}

void APlayerBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != nullptr)
	{
		IInteractionInterface* InteractInterface = Cast<IInteractionInterface>(OtherActor);

		if (InteractInterface != nullptr)
		{
			InteractInterface->Execute_SetInteractFocus(OtherActor);
			SetInteractableActor(OtherActor);
			
			APlayerControllerBase* PC = Cast<APlayerControllerBase>(GetController());
			if (PC != nullptr)
			{
				PC->EnableInteractText(OtherActor);
			}
		}

		ACheckpointActor* CheckpointActor = Cast<ACheckpointActor>(OtherActor);
		
		if (CheckpointActor != nullptr)
		{
			CheckpointReference = CheckpointActor;
		}

	}
}

void APlayerBase::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor != nullptr)
	{
		IInteractionInterface* InteractInterface = Cast<IInteractionInterface>(OtherActor);

		if (InteractInterface != nullptr)
		{
			InteractInterface->Execute_UnsetInteractFocus(OtherActor);
			SetInteractableActor(nullptr);

			APlayerControllerBase* PC = Cast<APlayerControllerBase>(GetController());
			if (PC != nullptr)
			{
				PC->DisableInteractText();
			}
		}
	}
}

void APlayerBase::PlantSeedAction_Implementation()
{
	if (AbilitySystemComponent.IsValid())
	{
		FGameplayTagContainer SeedTagContainer;
		SeedTagContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("Action.Plant")));
		AbilitySystemComponent->TryActivateAbilitiesByTag(SeedTagContainer);
	}
}

void APlayerBase::OnInteract_Implementation(AActor* Caller)
{
	if (!IsAlive())
	{
		Revive();
	}
}

void APlayerBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerBase, bCanAttack);
	DOREPLIFETIME(APlayerBase, InteractableActor);
	DOREPLIFETIME(APlayerBase, bCanSwitchToExploration);
	DOREPLIFETIME(APlayerBase, ChargeReleaseEvent);
}

FTransform APlayerBase::GetPivotTarget() const
{
	if (IsValid(GetMesh()) && GetMesh()->IsValidLowLevel())
	{
		FVector Head = GetMesh()->GetSocketLocation(TEXT("DEF_Head"));
		FVector Root = GetMesh()->GetSocketLocation(TEXT("Root"));

		return FTransform(GetActorRotation(), (Head + Root) / 2.0f, FVector::OneVector);
	}

	return FTransform::Identity;
}

ECollisionChannel APlayerBase::GetCameraTraceParams(FVector & TraceOrigin, float & TraceRadius) const
{
	if (IsValid(GetMesh()) && GetMesh()->IsValidLowLevel())
	{
		TraceOrigin = GetMesh()->GetSocketLocation(TEXT("DEF_Head"));
		TraceRadius = 15.0f;
	}

	return ECC_Camera;
}

void APlayerBase::GetCurrentCameraState(ERotationMode& OutRotationMode) const
{
	OutRotationMode = RotationMode;
}

void APlayerBase::SetInteractableActor_Implementation(AActor* ActorToInteract)
{
	InteractableActor = ActorToInteract;
}


void APlayerBase::OnCombatBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AEnemyBase* EnemyInRange = Cast<AEnemyBase>(OtherActor);

	if (EnemyInRange != nullptr)
	{
		EnemiesInRange.Add(EnemyInRange);
	}
}

void APlayerBase::OnCombatEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AEnemyBase* EnemyOutRange = Cast<AEnemyBase>(OtherActor);

	if (EnemyOutRange != nullptr)
	{
		if (EnemiesInRange.Contains(EnemyOutRange))
		{
			EnemiesInRange.Remove(EnemyOutRange);
		}
	}
}

void APlayerBase::AutoTargetingEnemy()
{
	AEnemyBase* EnemyToAttack = nullptr;

	SetPlayerRotationClient(FVector::ZeroVector);

	float DistanceToPlayerScalar = 1000000.0f;

	if (EnemiesInRange.Num())
	{
		for (AEnemyBase* EnemyInRange : EnemiesInRange)
		{
			EHitDirection DirectionOfEnemy = DefineDirectionHitted(EnemyInRange, this);

			if (DirectionOfEnemy == EHitDirection::FRONT)
			{		
				if (GetDistanceTo(EnemyInRange) < DistanceToPlayerScalar)
				{
					DistanceToPlayerScalar = GetDistanceTo(EnemyInRange);
					EnemyToAttack = EnemyInRange;
				}
				else
				{
					continue;
				}
			}
		}

		if (EnemyToAttack != nullptr)
		{
			RotateToTargetingEnemy(this, EnemyToAttack);
		}

		DistanceToPlayerScalar = 1000000.0f;
		EnemyToAttack = nullptr;
	}	
}

void APlayerBase::RotateToTargetingEnemy_Implementation(APlayerBase* PlayerToRotate, AEnemyBase* EnemyTarget)
{
	if (PlayerToRotate == nullptr || EnemyTarget == nullptr)
	{
		return;
	}

	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(PlayerToRotate->GetActorLocation(), EnemyTarget->GetActorLocation());
	FRotator NewRotation(0.0f, LookAtRotation.Yaw, LookAtRotation.Roll);
	PlayerToRotate->SetActorRotation(NewRotation);
	RotateToTargetingEnemyClient(PlayerToRotate, EnemyTarget);
}

void APlayerBase::RotateToTargetingEnemyClient_Implementation(APlayerBase* PlayerToRotate, AEnemyBase* EnemyTarget)
{
	if (PlayerToRotate == nullptr || EnemyTarget == nullptr)
	{
		return;
	}

	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(PlayerToRotate->GetActorLocation(), EnemyTarget->GetActorLocation());
	FRotator NewRotation(0.0f, LookAtRotation.Yaw, LookAtRotation.Roll);
	PlayerToRotate->SetActorRotation(NewRotation);
}


//////////////////////////////////////////////////////////////////////////
// Player Rotation Begin
//////////////////////////////////////////////////////////////////////////

void APlayerBase::RotatePlayer_Implementation(APlayerBase* PlayerToRotate, FRotator PlayerRotation)
{
	PlayerToRotate->SetActorRotation(PlayerRotation);
}

bool APlayerBase::RotatePlayer_Validate(APlayerBase* PlayerToRotate, FRotator PlayerRotation)
{
	return true;
}

void APlayerBase::RotatePlayerClient_Implementation(APlayerBase* PlayerToRotate)
{
	FRotator PlayerLastRotation = PlayerToRotate->GetActorRotation();
	if (!PlayerToRotate->DiretionToRotation.IsNearlyZero())
	{
		PlayerToRotate->SetActorRotation(UKismetMathLibrary::MakeRotFromX(PlayerToRotate->DiretionToRotation));
		RotatePlayer(this, UKismetMathLibrary::MakeRotFromX(PlayerToRotate->DiretionToRotation));
	}
}

void APlayerBase::SetPlayerRotationClient_Implementation(FVector RotationDirection)
{
	DiretionToRotation = RotationDirection;
}

//////////////////////////////////////////////////////////////////////////
// Player End
//////////////////////////////////////////////////////////////////////////

void APlayerBase::PlayCombatMusic_Implementation(const bool bIsBoss)
{
	const FName StateChange = bIsBoss == true ? "Final_Fight" : "Combat";
	UAkGameplayStatics::SetState("Gameplay", StateChange);
}

void APlayerBase::StopCombatMusic_Implementation()
{
	if (bCanSwitchToExploration == true)
	{
		UAkGameplayStatics::SetState("Gameplay", "Exploration");
	}
}

void APlayerBase::ResetDamageTaken()
{
	DamageTaken = 0.0f;
}

void APlayerBase::AddDamageTaken(const float CurrentDamageTaken)
{
	DamageTaken += CurrentDamageTaken;
}

float APlayerBase::GetDamageTaken() const
{
	return DamageTaken;
}

void APlayerBase::HandleCinematicMusicClient_Implementation(const bool bWillPlay, const FString& CinematicName)
{
	HandleCinematicMusic(bWillPlay, CinematicName);
}

void APlayerBase::HandleCinematicMusic(const bool bWillPlay, const FString& CinematicName)
{
	const FName CinematicCondition = bWillPlay == true ? "Yes" : "No";
	UAkGameplayStatics::SetState("Is_Cinematic", CinematicCondition);

	if (bWillPlay == true)
	{
		AkAudioComponent->SetSwitch("Cinematic_Type", CinematicName);
	}

	UAkAudioEvent* EventToPost = bWillPlay == true ? CinematicPlayEvent : CinematicStopEvent;

	if (EventToPost != nullptr)
	{
		FOnAkPostEventCallback NullCallback;
		TArray<FAkExternalSourceInfo> External;
		AkAudioComponent->PostAkEvent(EventToPost, int32(0), NullCallback, External, EventToPost->GetName());
	}
}

bool APlayerBase::HasCameraInput() const
{
	float MouseX = GetInputAxisValue("Turn");
	float MouseY = GetInputAxisValue("LookUp");

	const bool bHasKeyboardInput = MouseX || MouseY;
	const bool bIsInAimMode = RotationMode == ERotationMode::Aiming;

	return bHasKeyboardInput || bIsInAimMode;
}

void APlayerBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (AbilitySystemComponent.IsValid())
	{
		if (AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.BlockMove"))))
		{
			if (StopMoveEffect != nullptr)
			{
				FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
				EffectContext.AddSourceObject(this);

				FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(StopMoveEffect, 1, EffectContext);
				if (NewHandle.IsValid())
				{
					FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent.Get());
				}
			}
		}
		else
		{
			if (AddMoveEffect != nullptr)
			{
				FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
				EffectContext.AddSourceObject(this);

				FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(AddMoveEffect, 1, EffectContext);
				if (NewHandle.IsValid())
				{
					FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent.Get());
				}
			}
		}

		if (AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.BlockRotation"))))
		{
			bCanMove = false;
		}
		else
		{
			bCanMove = true;
		}

		if (AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Ability.Attack"))))
		{
			GetCharacterMovement()->bOrientRotationToMovement = false;
		}
		else
		{
			GetCharacterMovement()->bOrientRotationToMovement = true;
		}
	}

}


void APlayerBase::BindASCInput()
{
	if (!bASCInputBound && AbilitySystemComponent.IsValid() && IsValid(InputComponent))
	{
		AbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent, FGameplayAbilityInputBinds(FString("ConfirmTarget"),
			FString("CancelTarget"), FString("BGAbilityInputID"), static_cast<int32>(BGAbilityInputID::Confirm), static_cast<int32>(BGAbilityInputID::Cancel)));

		bASCInputBound = true;
	}
}

void APlayerBase::HandleHitResults(TArray<FHitResult> Hits)
{
	for (FHitResult hit : Hits)
	{
		AEnemyBase* CurrentEnemy = Cast<AEnemyBase>(hit.Actor);
		
		if (CurrentEnemy == nullptr)
		{
			return;
		}
		
		CurrentEnemy->SetLastAttackingCharacter(this);
		EHitDirection DiretionHitted = DefineDirectionHitted(this, CurrentEnemy);

		if (DiretionHitted == EHitDirection::FRONT && CurrentEnemy->CanBlock() == true)
		{
			CurrentEnemy->BlockAttack();
			return;
		}

		CurrentEnemy->OnDamageExecution();
		HandleDamageExecution(UAbilitySystemBlueprintLibrary::AbilityTargetDataFromHitResult(hit));

		OnHitDel.Broadcast(hit);
	}
}

void APlayerBase::PlayPlantSeedMontage()
{
	
}
