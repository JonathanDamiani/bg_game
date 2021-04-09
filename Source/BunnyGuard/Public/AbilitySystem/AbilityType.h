// Copyright (C) 2020 Alejandro G�ereca Valdivia / Jonathan Dean Damiani

#pragma once

#include "AbilityType.generated.h"

// Copyright (C) 2020 Alejandro G�ereca Valdivia / Jonathan Dean Damiani

UENUM(BlueprintType)
enum class EAbilityType : uint8
{
	BASIC_ATTACK	UMETA(DisplayName = "Basic Attack"),
	CHARGED_ATTACK	UMETA(DisplayName = "Charged Attack"),
	UTILITY			UMETA(DisplayName = "Utility Skill"),
	COMBAT			UMETA(DisplayName = "Combat Skill"),
	ULTIMATE		UMETA(DisplayName = "Ultimate Skill")
};
