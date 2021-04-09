// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EnemyMovementType.generated.h"
/**
 * 
 */
UENUM(BlueprintType)
enum class EEnemyMovementType : uint8
{
	Running	   = 0	 UMETA(DisplayName = "Running"),
	Straffing  = 1	 UMETA(DisplayName = "Straffing"),
	Reposition = 2   UMETA(DisplayName = "Reposition"),
	Sprinting  = 3   UMETA(DisplayName = "Sprinting")
};
