// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OptionsMenuBase.generated.h"

class UButtonBase;
class UWidgetSwitcher;

UCLASS()
class BUNNYGUARD_API UOptionsMenuBase : public UUserWidget
{
	GENERATED_BODY()

protected:
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (BindWidget))
	UButtonBase* Controls_Button;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (BindWidget))
	UWidgetSwitcher* Options_Switcher;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (BindWidget))
	UWidget* MainOptions_Box;

public:
	UFUNCTION(BlueprintCallable)
	void FocusButtonOnOpen();
};
