// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuBase.generated.h"

class UWidgetSwitcher;
class IMainMenuInterface;
class UButtonBase;
class UEditableText;

UCLASS()
class BUNNYGUARD_API UMainMenuBase : public UUserWidget
{
	GENERATED_BODY()

protected:

	//////////////////////////////////////////////////////////////////////////
	// Buttons

	UPROPERTY(meta = (BindWidget))
	UButtonBase* SinglePlayer_Button;

	UPROPERTY(meta = (BindWidget))
	UButtonBase* Multiplayer_Button;

	UPROPERTY(meta = (BindWidget))
	UButtonBase* Settings_Button;

	UPROPERTY(meta = (BindWidget))
	UButtonBase* Credits_Button;

	UPROPERTY(meta = (BindWidget))
	UButtonBase* HostGame_Button;

	UPROPERTY(meta = (BindWidget))
	UButtonBase* JoinGame_Button;
	
	UPROPERTY(meta = (BindWidget))
	UButtonBase* ReturnFromSettings_Button;

	UPROPERTY(meta = (BindWidget))
	UButtonBase* ReturnFromCredits_Button;

	UPROPERTY(meta = (BindWidget))
	UButtonBase* ReturnFromMultiplayer_Button;

	UPROPERTY(meta = (BindWidget))
	UButtonBase* ReturnFromServeList_Button;

	//////////////////////////////////////////////////////////////////////////
	// Switchers

	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* MainMenu_Switcher;

	UPROPERTY(meta = (BindWidget))
	UWidget* StartOptions_Box;

	UPROPERTY(meta = (BindWidget))
	UWidget* Settings_Box;

	UPROPERTY(meta = (BindWidget))
	UWidget* Credits_Box;

	UPROPERTY(meta = (BindWidget))
	UWidget* Multiplayer_Box;

	UPROPERTY(meta = (BindWidget))
	UWidget* ServeList_Box;

	UPROPERTY(EditAnywhere)
    FName LevelToLoadSinglePlayer;
	
	//////////////////////////////////////////////////////////////////////////
	// Functions

	UFUNCTION()
	void PlaySinglePlayer();

	UFUNCTION()
	void PlayerMultiplayer();

	UFUNCTION()
	void HostGame();

	UFUNCTION()
	void ShowCredits();

	UFUNCTION()
	void ShowServeList();

	UFUNCTION()
	void ShowSettings();

	UFUNCTION()
	void ReturnToMain();

	IMainMenuInterface* MainMenuInterface;

public:
	virtual bool Initialize() override;
	void SetInterface (IMainMenuInterface* InterfaceToSet);

	void SetFirstFocused();

	void DisableMultiplayer();

};
