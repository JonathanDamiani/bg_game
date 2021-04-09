// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PlayerType.generated.h"
/**
 * 
 */
UENUM(BlueprintType)
enum class EPlayerType : uint8
{
	Knight	  = 0	UMETA(DisplayName = "Knight"),
	Ranger    = 1   UMETA(DisplayName = "Ranger"),
	Mage	  = 2	UMETA(DisplayName = "Mage"),
	Fighter   = 3	UMETA(DisplayName = "Fighter"),
};
