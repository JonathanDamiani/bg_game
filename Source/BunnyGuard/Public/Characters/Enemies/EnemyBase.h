// Copyright (C) 2020 Alejandro G�ereca Valdivia / Jonathan Dean Damiani

#pragma once

#include "CoreMinimal.h"
#include "Characters/CharacterBase.h"
#include "GameplayEffectTypes.h"
#include "Characters/Enemies/Utils/EnemyType.h"
#include "EnemyBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAbilityTaskFinish, AEnemyBase*, Enemy);

class UBGAbilitySystemComponent;
class UAttributeSetBase;
class UFloatingHUDBase;
class UWidgetComponent;
class ATornadoBase;
class AVortexBase;
class URotatingMovementComponent;

UCLASS()
class BUNNYGUARD_API AEnemyBase : public ACharacterBase
{
	GENERATED_BODY()

public:
	AEnemyBase(const class FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Enemy")
	FOnAbilityTaskFinish OnAbilityTaskFinish;

	EEnemyType GetType() const;

	UFUNCTION()
	void SetTornadoReference(ATornadoBase* TornadoReference);

	UFUNCTION(BlueprintCallable)
	ATornadoBase* GetTornadoReference() const;

	UFUNCTION(BlueprintCallable)
	void StartTornadoBehaviour();

	UFUNCTION(BlueprintCallable)
	void EndTornadoBehaviour();

	UFUNCTION()
	void SetVortexReference(AVortexBase* VortexReference);
	
	UFUNCTION(BlueprintCallable)
	AVortexBase* GetVortexReference() const;

	UFUNCTION(BlueprintCallable)
	void StartVortexBehaviour();

	UFUNCTION(BlueprintCallable)
	void EndVortexBehaviour();

	UFUNCTION(BlueprintCallable)
	void ExecuteEnemyAbility(const bool bIsBasic) const; 

	UFUNCTION(BlueprintCallable)
	void BlockAttack();

	UFUNCTION(BlueprintCallable)
	bool CanBlock() const;

	UFUNCTION(BlueprintCallable)
	void SetStraffingSpeed();

	UFUNCTION(BlueprintCallable)
	void SetRepositionSpeed();

	UFUNCTION(BlueprintCallable)
	void SetSprintSpeed();

	UFUNCTION(BlueprintCallable)
	void ResetSpeed();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
	EEnemyType EnemyType = EEnemyType::Dagger;	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Abilities")
	TSubclassOf<UGameplayAbility> BasicAbility = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Abilities")
	TSubclassOf<UGameplayAbility> CombatAbility = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Sound)
	UAkAudioEvent* DissolveEvent = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Sound)
	UAkAudioEvent* BlockEvent = nullptr;

	UPROPERTY()
	UBGAbilitySystemComponent* EnemyAbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	URotatingMovementComponent* RotationMovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	URotatingMovementComponent* SelfRotationMovementComponent;

	FRotator CapsuleInitialRotation;

	UPROPERTY()
	UAttributeSetBase* EnemyAttributeSet;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
	TSubclassOf<UFloatingHUDBase> FloatingHUDClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Blocking")
	float TimeToLowerShield = 5.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float StrafeSpeed = 300.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float RepositionSpeed = 300.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float SprintSpeed = 900.0f;

	float DefaultSpeed = 600.0f;

	UPROPERTY()
	class UFloatingHUDBase* FloatingHUD;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UI")
	class UWidgetComponent* FloatingHUDComponent;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	ATornadoBase* TornadoTempReference;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	AVortexBase* VortexTempReference;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Enemy")
	bool bCanBlock;

	UPROPERTY(BlueprintReadWrite, Category = "Enemy")
	bool bIsBlocking = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Enemy")
	float TimeDoCompleteDeath = 3.0f;

	FDelegateHandle HealthChangedDelegateHandle;

	FGameplayTag RagdollTag;

	// Attribute changed callbacks
	virtual void HealthChanged(const FOnAttributeChangeData& Data);

	virtual void BeginPlay() override;

	virtual void Die() override;

	void FinalizeBlock();

	FTimerHandle TimeHandle_DeadTimerHandle;

	FTimerHandle ResetBlock_TimerHandle;

	virtual void HandleHitResults(TArray<FHitResult> Hits) override;

	bool bIsPendingDeath;
};
