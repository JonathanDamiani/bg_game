// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "Engine/EngineTypes.h"
#include "AbilitySlotUserWidgetBase.generated.h"

class UTextBlock;
class UImage;
class UAbilitySystemComponent;
class UMaterialInstanceDynamic;

UCLASS()
class BUNNYGUARD_API UAbilitySlotUserWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual bool Initialize() override;

	UFUNCTION(BlueprintCallable)
	void SetIconSlotFromAbility(FGameplayTagContainer InAbilityTag);

	UFUNCTION (BlueprintCallable)
	void StartIconCooldown(float CooldownTimeRemaining, float CooldownDuration);

	UFUNCTION (BlueprintCallable)
	void OnAbityFailToActive();

	UFUNCTION()
	void ManageIconCooldown();

	UFUNCTION()
	void ManageIconGlowEffect();

	UFUNCTION()
	void ManageIconFailEffect();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer AbilityTagSlotContainer;

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* HotKey_Text;

	UPROPERTY(EditAnywhere, Category="Default Values")
	FString HotKey;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* SlotIconGlow_Image;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* SlotIconBG_Image;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* SlotIcon_Image;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* SlotIconFront_Image;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* FailGlow_Image;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Cooldown_Text;

	FTimerHandle IconCooldownTimeHandle;
	FTimerDelegate IconCooldownTimerDelegate;

	float CooldownTimer;
	float CooldownFullDuration;

	FTimerHandle GlowEffectTimerHandle;

	UPROPERTY(BlueprintReadWrite,EditAnywhere, Category ="Glow Effect")
	float OpacityGlowPercentage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Glow Effect")
	float GlowChangeRatio;

	UMaterialInstanceDynamic* IconMaterial;

	void NativeConstruct() override;

	FTimerHandle IconFailActiveTimerHandle;
};
