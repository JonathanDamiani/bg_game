// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EnemyType.generated.h"
/**
 * 
 */
UENUM(BlueprintType)
enum class EEnemyType : uint8
{
	Dagger	  = 0	UMETA(DisplayName = "Dagger"),
	Ranger    = 1   UMETA(DisplayName = "Ranger"),
	Shield	  = 2	UMETA(DisplayName = "Shield"),
	Corrupt   = 3	UMETA(DisplayName = "Corrupt"),
	Wisp      = 4	UMETA(DisplayName = "Wisp"),
	Incarnate = 5	UMETA(DisplayName = "Incarnate"),
	None	  = 6   UMETA(DisplayName = "None")
};
