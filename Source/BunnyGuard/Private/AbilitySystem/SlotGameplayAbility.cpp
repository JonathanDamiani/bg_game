// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani


#include "AbilitySystem/SlotGameplayAbility.h"
#include "Materials/MaterialInterface.h"
#include "Engine/Texture.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystemComponent.h"
#include "UI/AbilitySlotUserWidgetBase.h"
#include "Characters/Player/PlayerControllerBase.h"

UMaterialInterface* USlotGameplayAbility::GetIconMaterialFromAbility() const
{
	return IconMaterial;
}

UTexture2D* USlotGameplayAbility::GetIconFromAbility() const
{
	return AbilityIcon;
}

FSlateColor USlotGameplayAbility::GetGlowColorFromAbility() const
{
	return AbilityGlowColor;
}

bool USlotGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags /*= nullptr*/, const FGameplayTagContainer* TargetTags /*= nullptr*/, OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr*/) const
{
	bool isAbleToActive = Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);

	if (!isAbleToActive)
	{
		APlayerControllerBase* PC = Cast<APlayerControllerBase>(ActorInfo->OwnerActor->GetOwner());
		if (PC != nullptr)
		{			
			PC->PlayAbilityFailToActiveHUD(ActivationOwnedTags);
		}

		return false;
	}

	return true;
}
