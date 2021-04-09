// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani


#include "UI/AbilitySlotUserWidgetBase.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbilitySpec.h"
#include "AbilitySystem/SlotGameplayAbility.h"
#include "Characters/Player/PlayerStateBase.h"
#include "Materials/MaterialInstanceDynamic.h"

bool UAbilitySlotUserWidgetBase::Initialize()
{
	bool bSuccess = Super::Initialize();

	if (!bSuccess) return false;

	if (HotKey_Text != nullptr)
	{
		HotKey_Text->SetText(FText::FromString(HotKey));
	}

	if (Cooldown_Text != nullptr)
	{
		Cooldown_Text->SetVisibility(ESlateVisibility::Hidden);
	}

	OpacityGlowPercentage = 1.0f;
	GlowChangeRatio = -0.05f;

	return true;
}

void UAbilitySlotUserWidgetBase::SetIconSlotFromAbility(FGameplayTagContainer InAbilityTag)
{
	APlayerStateBase* PS = Cast <APlayerStateBase>(GetOwningPlayerState());

	if (PS == nullptr)
	{
		return;
	}

	UAbilitySystemComponent* AbilitySystemComponent = Cast<UAbilitySystemComponent>(PS->GetAbilitySystemComponent());

	if (IsValid(AbilitySystemComponent) && InAbilityTag.Num() >= 1)
	{
		TArray<FGameplayAbilitySpec*> MatchingGameplayAbilities;

		AbilitySystemComponent->GetActivatableGameplayAbilitySpecsByAllMatchingTags(InAbilityTag, MatchingGameplayAbilities, true);

		if (MatchingGameplayAbilities.Num() > 0 && MatchingGameplayAbilities[0] != nullptr)
		{
			USlotGameplayAbility* AbilityFound = Cast<USlotGameplayAbility>(MatchingGameplayAbilities[0]->Ability);
			
			if (AbilityFound != nullptr)
			{
				if (SlotIcon_Image != nullptr)
				{
					IconMaterial = UMaterialInstanceDynamic::Create(AbilityFound->GetIconMaterialFromAbility(), this);
					SlotIcon_Image->SetBrushFromMaterial(IconMaterial);
				}

				if (SlotIconFront_Image != nullptr)
				{
					SlotIconFront_Image->SetBrushFromTexture(AbilityFound->GetIconFromAbility());
				}

				if (SlotIconGlow_Image != nullptr)
				{
					SlotIconGlow_Image->SetBrushTintColor(AbilityFound->GetGlowColorFromAbility());
				}
			}	
		}
	}
}

void UAbilitySlotUserWidgetBase::StartIconCooldown(float CooldownTimeRemaining, float CooldownDuration)
{
	CooldownTimer = CooldownTimeRemaining;
	CooldownFullDuration = CooldownDuration;

	if (CooldownTimer >= 0.0f && Cooldown_Text != nullptr)
	{
		IconCooldownTimerDelegate.BindUFunction(this, FName("ManageIconCooldown"));
		GetWorld()->GetTimerManager().SetTimer(IconCooldownTimeHandle, IconCooldownTimerDelegate, 0.1f, true);
		Cooldown_Text->SetText(FText::AsNumber(CooldownTimer));
		Cooldown_Text->SetVisibility(ESlateVisibility::Visible);
		SlotIconGlow_Image->SetVisibility(ESlateVisibility::Hidden);
		SlotIconFront_Image->SetOpacity(0.5f);
	}
}

void UAbilitySlotUserWidgetBase::ManageIconCooldown()
{
	FNumberFormattingOptions NumberFormatOptions;

	NumberFormatOptions.MaximumFractionalDigits = 0;
	Cooldown_Text->SetText(FText::AsNumber(CooldownTimer, &NumberFormatOptions));

	CooldownTimer -= 0.1f;

	if (IconMaterial != nullptr)
	{
		IconMaterial->SetScalarParameterValue(TEXT("PercentageCD"), 1.0f - (CooldownTimer / CooldownFullDuration));
	}
	
	if (CooldownTimer <= 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(IconCooldownTimeHandle);
		Cooldown_Text->SetText(FText::AsNumber(0.0f));
		Cooldown_Text->SetVisibility(ESlateVisibility::Hidden);
		SlotIconGlow_Image->SetVisibility(ESlateVisibility::Visible);
		SlotIconFront_Image->SetOpacity(1.0f);

		if (IconMaterial != nullptr)
		{
			IconMaterial->SetScalarParameterValue(TEXT("PercentageCD"), 1.0f);
		}
	}
}

void UAbilitySlotUserWidgetBase::ManageIconGlowEffect()
{
	if (OpacityGlowPercentage >= 1.1f || OpacityGlowPercentage <= 0.2f)
	{
		GlowChangeRatio *= -1;
	}

	OpacityGlowPercentage += GlowChangeRatio;

	SlotIconGlow_Image->SetOpacity(OpacityGlowPercentage);
}

void UAbilitySlotUserWidgetBase::OnAbityFailToActive()
{
	UWorld* World = GetWorld();

	if (World != nullptr)
	{	
		World->GetTimerManager().ClearTimer(IconFailActiveTimerHandle);

		if (FailGlow_Image != nullptr)
		{
			FailGlow_Image->SetOpacity(1.0f);
		}

		World->GetTimerManager().SetTimer(IconFailActiveTimerHandle, this, &UAbilitySlotUserWidgetBase::ManageIconFailEffect, 0.2f, true);
	}
}

void UAbilitySlotUserWidgetBase::ManageIconFailEffect()
{
	FailGlow_Image->SetOpacity(0.0f);
}

void UAbilitySlotUserWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();

	UWorld* World = GetWorld();

	if (World != nullptr)
	{
		World->GetTimerManager().SetTimer(GlowEffectTimerHandle, this, &UAbilitySlotUserWidgetBase::ManageIconGlowEffect, 0.1f, true);
	}
}
