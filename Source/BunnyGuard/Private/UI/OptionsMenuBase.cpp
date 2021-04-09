// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani


#include "UI/OptionsMenuBase.h"
#include "UI/ButtonBase.h"
#include "Components/WidgetSwitcher.h"

void UOptionsMenuBase::FocusButtonOnOpen()
{
	if (Options_Switcher != nullptr && MainOptions_Box != nullptr)
	{
		Options_Switcher->SetActiveWidget(MainOptions_Box);
	}

	if (Controls_Button != nullptr)
	{
		Controls_Button->SetButtonFocused();
	}
}
