// Copyright (C) 2020 Alejandro G�ereca Valdivia / Jonathan Dean Damiani

#pragma once

#include "CoreMinimal.h"
#include "Characters/CharacterBase.h"
#include "Gameplay/InteractionInterface.h"
#include "Characters/Player/Camera/RotationMode.h"
#include "Characters/Player/PlayerType.h"
#include "GameplayEffect.h"
#include "PlayerBase.generated.h"

class APlayerControllerBase;
class UTextRenderComponent;
class USphereComponent;
class ACheckpointActor;
class AEnemyBase;
class ATargetBase;
class UAnimMontage;

UCLASS()
class BUNNYGUARD_API APlayerBase : public ACharacterBase, public IInteractionInterface
{
	GENERATED_BODY()

public:
	APlayerBase(const class FObjectInitializer& ObjectInitializer);

	virtual void ActiveAbilityHandle(EAbilityType AbilityTypeEnum) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void Die() override;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<ATargetBase> ArrowActorToOtherPlayer;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<ATargetBase*> ArrowsToOtherPlayers;

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void FindOtherAndPointToThem();
	void FindOtherAndPointToThem_Implementation();

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	virtual void Revive();
	virtual void Revive_Implementation();

	UFUNCTION(BlueprintCallable)
	virtual void SetPlayerCanMove(bool bIsPlayerCanMove);

	UFUNCTION(BlueprintCallable)
	virtual void DisableAbilities();

	UFUNCTION(BlueprintCallable)
	virtual void EnableAbilities();

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void ShowEndGameScreen(bool bIsVictory);
	void ShowEndGameScreen_Implementation(bool bIsVictory);

	UFUNCTION(Server, Reliable)
	void HandleInteractions();
	void HandleInteractions_Implementation();

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void HandleCheckpoint();
	void HandleCheckpoint_Implementation();

	UFUNCTION(BlueprintCallable)
	virtual void HandleAimPressed();
	
	UFUNCTION(BlueprintCallable)
	virtual void HandleAimReleased();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTextRenderComponent* TextReviveComponent;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(NetMulticast, Reliable)
	void PlantSeedAction();
	void PlantSeedAction_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Interact")
	void OnInteract(AActor* Caller);
	void OnInteract_Implementation(AActor* Caller);

	void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty> & OutLifetimeProps) const override;
	FTransform GetPivotTarget() const;

	ECollisionChannel GetCameraTraceParams(FVector& TraceOrigin, float& TraceRadius) const;

	UFUNCTION(BlueprintCallable)
	void GetCurrentCameraState(ERotationMode& OutRotationMode) const;

	UFUNCTION(Server, Reliable)
	void SetInteractableActor(AActor* ActorToInteract);
	void SetInteractableActor_Implementation(AActor* ActorToInteract);

	UFUNCTION(BlueprintCallable, Client, Reliable, Category = "Sound")
	void PlayCombatMusic(const bool bIsBoss);
	void PlayCombatMusic_Implementation(const bool bIsBoss);

	UFUNCTION(BlueprintCallable, Client, Reliable, Category = "Sound")
	void StopCombatMusic();
	void StopCombatMusic_Implementation();

	UFUNCTION(BlueprintCallable, Client, Reliable, Category = "Sound")
	void HandleCinematicMusicClient(const bool bWillPlay, const FString& CinematicName);
	void HandleCinematicMusicClient_Implementation(const bool bWillPlay, const FString& CinematicName);

	UFUNCTION(BlueprintCallable, Category = "Sound")
	void HandleCinematicMusic(const bool bWillPlay, const FString& CinematicName);

	bool HasCameraInput() const;

	void ResetDamageTaken();

	void AddDamageTaken(const float CurrentDamageTaken);

	float GetDamageTaken() const;

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION()
	void OnCombatBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnCombatEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	 
	UFUNCTION(BlueprintCallable)
	void AutoTargetingEnemy();

	UFUNCTION()
	void PlayPlantSeedMontage();

	UFUNCTION(BlueprintCallable, Server, Unreliable)
	void RotateToTargetingEnemy(APlayerBase* PlayerToRotate, AEnemyBase* EnemyTarget);
	void RotateToTargetingEnemy_Implementation(APlayerBase* PlayerToRotate, AEnemyBase* EnemyTarget);

	UFUNCTION(BlueprintCallable, Client, Unreliable)
	void RotateToTargetingEnemyClient(APlayerBase* PlayerToRotate, AEnemyBase* EnemyTarget);
	void RotateToTargetingEnemyClient_Implementation(APlayerBase* PlayerToRotate, AEnemyBase* EnemyTarget);

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void RotatePlayer(APlayerBase* PlayerToRotate, FRotator PlayerRotation);
	void RotatePlayer_Implementation(APlayerBase* PlayerToRotate, FRotator PlayerRotation);

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void RotatePlayerClient(APlayerBase* PlayerToRotate);
	void RotatePlayerClient_Implementation(APlayerBase* PlayerToRotate);

	UFUNCTION(Client, Reliable)
	void SetPlayerRotationClient(FVector RotationDirection);

	int32 TelemetryId = -1;

	UPROPERTY(Replicated, BlueprintReadOnly)
	AActor* InteractableActor;

	UFUNCTION(Server, Reliable)
	void UpdatePlayersInfoServer();
	void UpdatePlayersInfoServer_Implementation();

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	bool bCanSwitchToExploration = true;

	UFUNCTION(BlueprintCallable)
	EPlayerType GetPlayerType() const;

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void PlayChargeReleaseSFX();
	void PlayChargeReleaseSFX_Implementation();

protected: 

	//////////////////////////////////////////////////////////////////////////
	// Properties

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPlayerType PlayerType = EPlayerType::Knight; 

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USphereComponent* InteractCollider;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USphereComponent* CombatCheckCollider;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<AEnemyBase*> EnemiesInRange;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Sound)
	UAkAudioEvent* ReviveEvent = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category = Sound)
	UAkAudioEvent* ChargeReleaseEvent = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Sound)
	class UAkAudioEvent* CinematicPlayEvent = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Sound)
	class UAkAudioEvent* CinematicStopEvent = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Death / Revive")
	UAnimMontage* DeathMontage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Death / Revive")
	UAnimMontage* ReviveMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> AddMoveEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> StopMoveEffect;

	UPROPERTY(BlueprintReadWrite)
	bool bCanMove;

	UPROPERTY(BlueprintReadWrite, Replicated)
	bool bCanAttack = false;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);

	void MoveRight(float Value);
	
	virtual void HandleHitResults(TArray<FHitResult> Hits) override;

	// Client only
	virtual void OnRep_PlayerState() override;

	bool bASCInputBound = false;

	FGameplayTag DeadTag;
	FGameplayTag DisableAbilitiesTag;

	void BindASCInput();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ACheckpointActor* CheckpointReference;

	ERotationMode RotationMode = ERotationMode::Velocity;

	FVector DiretionToRotation;

	float DamageTaken = 0;
};
