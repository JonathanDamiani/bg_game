// Copyright (C) 2020 Alejandro G�ereca Valdivia / Jonathan Dean Damiani

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "AbilitySystem/AbilityType.h"
#include "AbilitySystem/HitDirection.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "CharacterBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterDiedDelegate, ACharacterBase*, Character);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHitDelegate, FHitResult, CharacterHitResult);

class UAbilitySystemComponent;
class UAttributeSetBase;
class UBGAbilitySystemComponent;
class UGameplayAbilityBase;
class UGameplayEffect;
class UAkComponent;
class UAkAudioEvent;
class UNiagaraSystem;

UCLASS()
class BUNNYGUARD_API ACharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharacterBase(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintAssignable, Category="CharacterBase")
	FCharacterDiedDelegate OnCharacterDie;

	UPROPERTY(BlueprintAssignable, Category="CharacterBase")
	FOnHitDelegate OnHitDel;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	//////////////////////////////////////////////////////////////////////////
	// Helpers methods

	UFUNCTION(BlueprintCallable, Category = "CharacterAttributes")
	virtual	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "CharacterAttributes")
	virtual	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "CharacterAttributes")
	virtual	float GetMoveSpeed() const;

	UFUNCTION(BlueprintCallable, Category = "TPCharacter")
	virtual bool IsAlive() const;

	UFUNCTION(BlueprintCallable, Category = "TPCharacter")
	virtual void Die();

	UFUNCTION(BlueprintImplementableEvent)
	void OnDamageExecution();

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void HandleDamageExecution(FGameplayAbilityTargetDataHandle TargeToDamage);
	virtual void HandleDamageExecution_Implementation(FGameplayAbilityTargetDataHandle TargeToDamage);

	UFUNCTION(BlueprintCallable)
	virtual void RemoveCharacterAbilities();

	UFUNCTION(BlueprintCallable)
	EHitDirection DefineDirectionHitted(AActor* CharacterInstigator, AActor* CharacterDamaged);

	virtual void PlayHitSFX();

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void OnHitEvent(FHitResult CharacterHitResult);
	virtual void OnHitEvent_Implementation(FHitResult CharacterHitResult);

	UFUNCTION(BlueprintCallable)
	ACharacterBase* GetLastAttackingCharacter() const;

	UFUNCTION(BlueprintCallable)
	void SetLastAttackingCharacter(ACharacterBase* AttackingActor);

	UFUNCTION(BlueprintCallable)
	void PlayHitEffect(FVector position);

	UFUNCTION(BlueprintCallable)
	void SetHitVFX(UNiagaraSystem* HitVFXToChange, float HitVFXSizeScale);

	UFUNCTION(BlueprintCallable)
	FString GetSFXCharacterState() const;

protected:

	//////////////////////////////////////////////////////////////////////////
	// Properties

	TWeakObjectPtr<UBGAbilitySystemComponent> AbilitySystemComponent;
	TWeakObjectPtr<UAttributeSetBase> AttributeSetBase;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
	UNiagaraSystem* HitVFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
	float HitSFXScale;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbilityBase>> StartAbilities;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	TSubclassOf<UGameplayEffect> DefaultAttributes;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UGameplayEffect>> StartupEffects;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category ="Collision Damage")
	TEnumAsByte<ECollisionChannel> CollisionChannelToDamage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Sound)
	UAkComponent* AkAudioComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Sound)
	UAkAudioEvent* FaintEvent = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Sound)
	UAkAudioEvent* GetHitEvent = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Sound)
	FString FootstepState = "Leather_Shoes";

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Sound)
	FString CharacterState = "Archer";

	//////////////////////////////////////////////////////////////////////////
	// Ability System

	FGameplayTag DeadTag;
	FGameplayTag EffectRemoveOnDeathTag;
	FGameplayTag BlockMoveTag;

	virtual void AddCharacterAbilities();

	virtual void InitializeAttributes();

	virtual void AddStartupEffects();

	virtual void SetHealth(float Health);

	//////////////////////////////////////////////////////////////////////////
	// Ability handles

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category="Abilities Handle")
	virtual void ActiveAbilityHandle(EAbilityType AbilityTypeEnum);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Abilities Handle")
	void ExecuteBasicAttack();
	virtual void ExecuteBasicAttack_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Abilities Handle")
	void ExecuteChargedAttack();
	virtual void ExecuteChargedAttack_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Abilities Handle")
	void ExecuteUtility();
	virtual void ExecuteUtility_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Abilities Handle")
	void ExecuteCombat();
	virtual void ExecuteCombat_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Abilities Handle")
	void ExecuteUltimate();
	virtual void ExecuteUltimate_Implementation();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void HandleReactionBehaviour(EHitDirection EDirectionHitted, ACharacterBase* CharacterToReact);
	virtual void HandleReactionBehaviour_Implementation(EHitDirection EDirectionHitted, ACharacterBase* CharacterToReact);

	UFUNCTION(BlueprintCallable)
	void SphereAttack(float StartMultiplier, float EndMultiplier, float RadiusOfAttack, bool EnableDebug);

	UFUNCTION(BlueprintCallable)
	virtual void HandleHitResults(TArray<FHitResult> Hits);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void HandleKnockbackReaction(ACharacterBase* CharacterThatHittedMe);
	virtual void HandleKnockbackReaction_Implementation(ACharacterBase* CharacterThatHittedMe);

	UPROPERTY(BlueprintReadWrite)
	bool InKnockBack;

	ACharacterBase* LastAttackingCharacter;

	FString AbilityHittedBySwitch;
};
