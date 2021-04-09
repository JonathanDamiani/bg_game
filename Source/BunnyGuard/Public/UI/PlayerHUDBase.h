// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/AbilitySlotUserWidgetBase.h"
#include "GameplayTagContainer.h"
#include "PlayerInfo.h"
#include "PlayerHUDBase.generated.h"

class UVerticalBox;
class UPlayersInfoHUD;

UCLASS()
class BUNNYGUARD_API UPlayerHUDBase : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetMaxHealth(float MaxHealth);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetCurrentHealth(float CurrentHealth);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetHealthPercentage(float HealthPercentage);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetAllIconsWidgets();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetBossHealthPercentage(float HealthPercentage);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetVisibilityOfCrosshair(bool bIsCrosshairVisible);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<UAbilitySlotUserWidgetBase*> SlotsAbilities;

	UFUNCTION(BlueprintCallable)
	void PlayAbilityFailToActive(FGameplayTagContainer InAbilityTag);

};
