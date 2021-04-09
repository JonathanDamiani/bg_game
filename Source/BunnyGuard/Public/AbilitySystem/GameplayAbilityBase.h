// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "BunnyGuard/BunnyGuard.h"
#include "GameplayAbilityBase.generated.h"

class UNiagaraSystem;
UCLASS()
class BUNNYGUARD_API UGameplayAbilityBase : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGameplayAbilityBase();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Ability")
	BGAbilityInputID AbilityInputID = BGAbilityInputID::None;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Ability")
	BGAbilityInputID AbilityID = BGAbilityInputID::None;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
	bool bActiveAbilityOnGranted = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
	UNiagaraSystem* HitVFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
	float HitSFXScale;

	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
};
