// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "PlayerInfo.h"
#include "Net/UnrealNetwork.h"
#include "PlayerStateBase.generated.h"

class UAbilitySystemComponent;
class UAttributeSetBase;
class UBGAbilitySystemComponent;

UCLASS()
class BUNNYGUARD_API APlayerStateBase : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	APlayerStateBase();

	UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UAttributeSetBase* GetAttributeSet() const;
	
	UFUNCTION(BlueprintCallable, Category = "CharacterState")
	bool IsAlive() const;

	UFUNCTION(BlueprintCallable, Category = "PlayerAttributes")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "PlayerAttributes")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "PlayerAttributes")
	float GetHealthRegenRate() const;

	UFUNCTION(BlueprintCallable, Category = "PlayerAttributes")
	float GetMoveSpeed() const;

	UFUNCTION(BlueprintCallable, Category = "Player info")
	void SetPlayerInfo();

	void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty> & OutLifetimeProps) const override;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Replicated, Category = "Character Selected")
	EPlayerCharacterClass ClassSelected;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Replicated, Category = "Player info")
	FPlayerInfo ThisPlayerInfo;

protected:
	FGameplayTag DeadTag;

	FGameplayTag DisableAbilitiesTag;

	UPROPERTY()
	UBGAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	UAttributeSetBase* AttributeSetBase;

	virtual void BeginPlay() override;

	FDelegateHandle HealthChangedDelegateHandle;
	FDelegateHandle MaxHealthChangedDelegateHandle;
	FDelegateHandle HealthRegenRateChangedDelegateHandle;

	virtual void HealthChanged(const FOnAttributeChangeData& Data);
	virtual void MaxHealthChanged(const FOnAttributeChangeData& Data);
	virtual void HealthRegenRateChanged(const FOnAttributeChangeData& Data);
};
