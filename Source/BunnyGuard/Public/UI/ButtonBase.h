// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Engine/EngineTypes.h"
#include "ButtonBase.generated.h"

class UTextBlock;

UCLASS()
class BUNNYGUARD_API UButtonBase : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual bool Initialize() override;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (BindWidget))
	UButton* Base_Button;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* BaseButton_Text;

	UPROPERTY(EditAnywhere, Category="Default Values")
	FString ButtonText ="";

	FTimerHandle FocusedButtonTimerHandle;

	float TimeToCheckBackground;

	UFUNCTION(BlueprintCallable)
	void ToggleButtonSelection();

	UFUNCTION(BlueprintCallable)
	void SetButonActive(bool bActived);

	UFUNCTION(BlueprintCallable)
	bool IsButtonActive() const;
	
	UFUNCTION(BlueprintCallable)
	void SetButtonFocused() const;

private:
	bool bIsActiveButton = false;
};
