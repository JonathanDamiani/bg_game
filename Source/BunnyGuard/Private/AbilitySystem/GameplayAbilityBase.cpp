// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani


#include "AbilitySystem/GameplayAbilityBase.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "Niagara/Public/NiagaraComponent.h"
#include "Niagara/Public/NiagaraFunctionLibrary.h"

UGameplayAbilityBase::UGameplayAbilityBase()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Dead")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Debuff.Stun")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.DisableAbilities")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Jump")));
}

void UGameplayAbilityBase::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);
	
	if (bActiveAbilityOnGranted)
	{
		bool ActivatedAbility = ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle, false);
	}
}
