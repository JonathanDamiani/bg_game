// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"

UENUM(BlueprintType)
enum class BGAbilityInputID : uint8
{	
	None			UMETA(DisplayName = "None"),
	Confirm			UMETA(DisplayName = "Confirm"),
	Cancel			UMETA(DisplayName = "Cancel"),
	BasicAttack		UMETA(DisplayName = "BasicAttack"),
	Interact		UMETA(DisplayName = "Interact"),
	UsePotion		UMETA(DisplayName = "UsePotion"),
	AimLock			UMETA(DisplayName = "AimLock"),
	UtilitySkill	UMETA(DisplayName = "UtilitySkill"),
	CombatSkill		UMETA(DisplayName = "CombatSkill"),
	UltimateSkill	UMETA(DisplayName = "UltimateSkill"),
	DodgeRoll		UMETA(DisplayName = "DodgeRoll"),
	Sprint			UMETA(DisplayName = "Sprint"),
	ResetCamera		UMETA(DisplayName = "ResetCamera"),
	OpenOptions		UMETA(DisplayName = "OpenOptions"),
	Jump			UMETA(DisplayName = "Jump"),
};
