// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani


#include "UI/PlayerHUDBase.h"
#include "Components/VerticalBox.h"
#include "UI/PlayersInfoHUD.h"
#include "Characters/Player/PlayerStateBase.h"

void UPlayerHUDBase::PlayAbilityFailToActive(FGameplayTagContainer InAbilityTag)
{
	for (UAbilitySlotUserWidgetBase* SlotAbility : SlotsAbilities)
	{
		if (SlotAbility != nullptr)
		{
			if (SlotAbility->AbilityTagSlotContainer.HasAny(InAbilityTag))
			{
				SlotAbility->OnAbityFailToActive();
			}
		}
	}
}
