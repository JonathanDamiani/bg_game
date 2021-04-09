// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GameplayAbilityBase.h"
#include "Styling/SlateColor.h"
#include "SlotGameplayAbility.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAbilityFailDelegate);

class UMaterialInterface;
class UTexture;
class UAbilitySlotUserWidgetBase;

UCLASS()
class BUNNYGUARD_API USlotGameplayAbility : public UGameplayAbilityBase
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Ability")
	UMaterialInterface* IconMaterial;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Ability")
	UTexture2D* AbilityIcon;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
	FSlateColor AbilityGlowColor;
	
	UMaterialInterface* GetIconMaterialFromAbility() const;

	UTexture2D* GetIconFromAbility() const;

	FSlateColor GetGlowColorFromAbility() const;

	UPROPERTY(BlueprintAssignable)
	FAbilityFailDelegate OnAbilityFailToActive;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UAbilitySlotUserWidgetBase* ReferenceToWidget;

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const;
};
