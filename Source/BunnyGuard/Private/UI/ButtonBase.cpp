// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani


#include "UI/ButtonBase.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

bool UButtonBase::Initialize()
{
	bool bSuccess = Super::Initialize();
	
	if (!bSuccess)
	{
		return false;
	}

	if (BaseButton_Text != nullptr)
	{
		BaseButton_Text->SetText(FText::FromString(ButtonText));
	}

	return true;
}

void UButtonBase::ToggleButtonSelection()
{
	bIsActiveButton = bIsActiveButton ? false : true;
}

void UButtonBase::SetButonActive(bool bActived)
{
	bIsActiveButton = bActived;
}

bool UButtonBase::IsButtonActive() const
{
	return bIsActiveButton;
}

void UButtonBase::SetButtonFocused() const
{
	Base_Button->SetFocus();
}
