// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani


#include "UI/LobbyMenuBase.h"
#include "UI/LobbyMenuInterface.h"
#include "Components/EditableText.h"
#include "Components/VerticalBox.h"
#include "Kismet/GameplayStatics.h"
#include "Characters/Player/PlayerStateBase.h"
#include "BGGameState.h"
#include "UI/PlayerInfoItemWidget.h"
#include "UI/ButtonBase.h"


bool ULobbyMenuBase::Initialize()
{
	bool bSuccess = Super::Initialize();
	
	if (!bSuccess) 
	{
		return false;
	}
	
	if (IsReady_Button != nullptr)
	{
		IsReady_Button->SetIsEnabled(false);
	}

	if (Knight_Button != nullptr)
	{
		CharacterSelectionButtons.Add(Knight_Button);
		ButtonsMaps.Add(EPlayerCharacterClass::Knight, Knight_Button);
	}

	if (Fighter_Button != nullptr)
	{
		CharacterSelectionButtons.Add(Fighter_Button);
		ButtonsMaps.Add(EPlayerCharacterClass::Fighter, Fighter_Button);
	}

	if (Ranger_Button != nullptr)
	{
		CharacterSelectionButtons.Add(Ranger_Button);
		ButtonsMaps.Add(EPlayerCharacterClass::Ranger, Ranger_Button);
	}

	if (Mage_Button != nullptr)
	{
		CharacterSelectionButtons.Add(Mage_Button);
		ButtonsMaps.Add(EPlayerCharacterClass::Mage, Mage_Button);
	}

	return true;
}

void ULobbyMenuBase::SelectCharacter(EPlayerCharacterClass ECharaterSelected)
{
	switch (ECharaterSelected)
	{
	case EPlayerCharacterClass::None:

		EThisSelectedCharacter = EPlayerCharacterClass::None;

		break;
	case EPlayerCharacterClass::Knight:

		Knight_Button->ToggleButtonSelection();
		UpdateButtonsActive(Knight_Button);

		break;
	case EPlayerCharacterClass::Fighter:

		Fighter_Button->ToggleButtonSelection();
		UpdateButtonsActive(Fighter_Button);

		break;
	case EPlayerCharacterClass::Ranger:

		Ranger_Button->ToggleButtonSelection();
		UpdateButtonsActive(Ranger_Button);

		break;
	case EPlayerCharacterClass::Mage:

		Mage_Button->ToggleButtonSelection();
		UpdateButtonsActive(Mage_Button);

		break;
	default:
		break;
	}

	EThisSelectedCharacter = ECharaterSelected;
	ActivePlayerReady();
}

void ULobbyMenuBase::UpdateAllSelectedCharacters(TArray<FPlayerInfo> PlayersInfo)
{

	for (int i = (int)EPlayerCharacterClass::Knight; i <= int(EPlayerCharacterClass::Mage); i++)
	{
		ButtonsMaps[(EPlayerCharacterClass)i]->Base_Button->SetIsEnabled(true);
	}

	for (FPlayerInfo playerInfo : PlayersInfo)
	{
		if (playerInfo.NetPlayerClass == EThisSelectedCharacter)
		{
			continue;
		}

		if (!ButtonsMaps.Contains(playerInfo.NetPlayerClass))
		{
			continue;
		}

		ButtonsMaps[playerInfo.NetPlayerClass]->Base_Button->SetIsEnabled(false);
	}
}

void ULobbyMenuBase::UpdateButtonsActive(UButtonBase* ButtonToIgnore)
{
	for (UButtonBase* button : CharacterSelectionButtons)
	{
		if (button == ButtonToIgnore)
		{
			continue;
		}

		if (button->IsButtonActive())
		{
			button->SetButonActive(false);
		}
	}
}

bool ULobbyMenuBase::CheckPlayerReady(TArray<FPlayerInfo> PlayersInfo)
{
	for (FPlayerInfo playerInfo : PlayersInfo)
	{
		if (!playerInfo.IsPlayerReady)
		{
			return false;
		}
	}
	return true;
}

void ULobbyMenuBase::ActivePlayerReady()
{
	if (EThisSelectedCharacter != EPlayerCharacterClass::None)
	{
		IsReady_Button->SetIsEnabled(true);
	}
	else
	{
		IsReady_Button->SetIsEnabled(false);
	}
}

void ULobbyMenuBase::SetLobbyInterface(ILobbyMenuInterface* InterfaceToSet)
{
	LobbyMenuInterface = InterfaceToSet;
}
