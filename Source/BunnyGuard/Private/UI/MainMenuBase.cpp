// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani


#include "UI/MainMenuBase.h"
#include "UI/ButtonBase.h"
#include "Kismet/GameplayStatics.h"
#include "Components/WidgetSwitcher.h"
#include "Components/EditableText.h"
#include "UI/MainMenuInterface.h"

bool UMainMenuBase::Initialize()
{
	bool bSuccess = Super::Initialize();

	if (!bSuccess) 
	{
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	// Main Buttons

	if (SinglePlayer_Button != nullptr)
	{
		SinglePlayer_Button->Base_Button->OnClicked.AddDynamic(this, &UMainMenuBase::PlaySinglePlayer);
	}

	if (Multiplayer_Button != nullptr)
	{
		Multiplayer_Button->Base_Button->OnClicked.AddDynamic(this, &UMainMenuBase::PlayerMultiplayer);
	}

	if (Settings_Button != nullptr)
	{
		Settings_Button->Base_Button->OnClicked.AddDynamic(this, &UMainMenuBase::ShowSettings);
	}

	if (Credits_Button != nullptr)
	{
		Credits_Button->Base_Button->OnClicked.AddDynamic(this, &UMainMenuBase::ShowCredits);
	}

	if (HostGame_Button != nullptr)
	{
		HostGame_Button->Base_Button->OnClicked.AddDynamic(this, &UMainMenuBase::HostGame);
	}

	if (JoinGame_Button != nullptr)
	{
		JoinGame_Button->Base_Button->OnClicked.AddDynamic(this, &UMainMenuBase::ShowServeList);
	}

	//////////////////////////////////////////////////////////////////////////
	// Return Buttons

	if (ReturnFromSettings_Button != nullptr)
	{
		ReturnFromSettings_Button->Base_Button->OnClicked.AddDynamic(this, &UMainMenuBase::ReturnToMain);
	}

	if (ReturnFromCredits_Button != nullptr)
	{
		ReturnFromCredits_Button->Base_Button->OnClicked.AddDynamic(this, &UMainMenuBase::ReturnToMain);
	}

	if (ReturnFromMultiplayer_Button != nullptr)
	{
		ReturnFromMultiplayer_Button->Base_Button->OnClicked.AddDynamic(this, &UMainMenuBase::ReturnToMain);
	}

	if (ReturnFromServeList_Button != nullptr)
	{
		ReturnFromServeList_Button->Base_Button->OnClicked.AddDynamic(this, &UMainMenuBase::PlayerMultiplayer);
	}

	return true;
}


void UMainMenuBase::PlaySinglePlayer()
{
	UGameplayStatics::OpenLevel(this, LevelToLoadSinglePlayer, true);
}

void UMainMenuBase::PlayerMultiplayer()
{
	if (MainMenu_Switcher != nullptr && Multiplayer_Box != nullptr)
	{
		MainMenu_Switcher->SetActiveWidget(Multiplayer_Box);
	}
}

void UMainMenuBase::HostGame()
{
	MainMenuInterface->Host();
}

void UMainMenuBase::ShowCredits()
{
	if (MainMenu_Switcher != nullptr && Credits_Box != nullptr)
	{
		MainMenu_Switcher->SetActiveWidget(Credits_Box);
	}
}

void UMainMenuBase::ShowServeList()
{
	if (MainMenu_Switcher != nullptr && ServeList_Box != nullptr)
	{
		MainMenu_Switcher->SetActiveWidget(ServeList_Box);
	}

	MainMenuInterface->OpenSessionListMenu();
}

void UMainMenuBase::ShowSettings()
{
	if (MainMenu_Switcher != nullptr && Settings_Box != nullptr)
	{
		MainMenu_Switcher->SetActiveWidget(Settings_Box);
	}
}

void UMainMenuBase::ReturnToMain()
{
	if (MainMenu_Switcher != nullptr && StartOptions_Box != nullptr)
	{
		MainMenu_Switcher->SetActiveWidget(StartOptions_Box);
	}
}

void UMainMenuBase::SetFirstFocused()
{
	SinglePlayer_Button->Base_Button->SetFocus();
}

void UMainMenuBase::DisableMultiplayer()
{
	if (Multiplayer_Button != nullptr)
	{
		Multiplayer_Button->Base_Button->SetIsEnabled(false);
	}
}

void UMainMenuBase::SetInterface(IMainMenuInterface* InterfaceToSet)
{
	MainMenuInterface = InterfaceToSet;
}
