// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani


#include "UI/PlayerInfoItemWidget.h"
#include "Components/TextBlock.h"

void UPlayerInfoItemWidget::SetPlayerNameUI(FText playerName)
{
	PlayerName_Text->SetText(playerName);
}

void UPlayerInfoItemWidget::SetPlayerClassUI(EPlayerCharacterClass EPlayerClass)
{
	switch (EPlayerClass)
	{
	case EPlayerCharacterClass::None:
		PlayerClass_Text->SetText(FText::FromString("None"));
		break;
	case EPlayerCharacterClass::Knight:
		PlayerClass_Text->SetText(FText::FromString("Knight"));
		break;
	case EPlayerCharacterClass::Fighter:
		PlayerClass_Text->SetText(FText::FromString("Fighter"));
		break;
	case EPlayerCharacterClass::Ranger:
		PlayerClass_Text->SetText(FText::FromString("Ranger"));
		break;
	case EPlayerCharacterClass::Mage:
		PlayerClass_Text->SetText(FText::FromString("Mage"));
		break;
	default:
		break;
	}
	
}

void UPlayerInfoItemWidget::SetPlayerReady(bool bIsPlayerReady)
{
	if (bIsPlayerReady)
	{
		PlayerReady_Text->SetColorAndOpacity(TextReadyColor);
		PlayerReady_Text->SetText(FText::FromString("Ready"));
	}
	else
	{	
		PlayerReady_Text->SetColorAndOpacity(TextNotReadyColor);
		PlayerReady_Text->SetText(FText::FromString("Not Ready"));
	}
}

void UPlayerInfoItemWidget::SetAllPlayerInfo(FPlayerInfo AllInfo)
{
	SetPlayerNameUI(AllInfo.NetPlayerName);
	SetPlayerClassUI(AllInfo.NetPlayerClass);
	SetPlayerReady(AllInfo.IsPlayerReady);
}
