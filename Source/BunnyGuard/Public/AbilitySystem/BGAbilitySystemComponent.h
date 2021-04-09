// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "BGAbilitySystemComponent.generated.h"

UCLASS()
class BUNNYGUARD_API UBGAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:

	bool bCharacterAbilitiesGiven = false;
	bool bStartupEffectsApplied = false;
};
