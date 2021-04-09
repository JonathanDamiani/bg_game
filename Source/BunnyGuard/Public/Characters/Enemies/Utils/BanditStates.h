// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BanditStates.generated.h"
/**
 * 
 */
UENUM(BlueprintType)
enum class EBanditStates : uint8
{
	Repositon     = 0	UMETA(DisplayName = "Repositon"),
	Basic_Attack  = 1   UMETA(DisplayName = "Basic Attack"),
	Combat_Attack = 2   UMETA(DisplayName = "Combat Attack"),
	Strafe	      = 3   UMETA(DisplayName = "Strafe"),
	Taunt	      = 4   UMETA(DisplayName = "Taunt ")
};
