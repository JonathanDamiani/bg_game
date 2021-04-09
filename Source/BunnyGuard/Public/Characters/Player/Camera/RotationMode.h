// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani

#pragma once

#include "RotationMode.generated.h"
/**
 *
 */
UENUM(BlueprintType)
enum class ERotationMode : uint8
{
	Velocity = 0	UMETA(DisplayName = "Velocity"),
	Aiming   = 1	UMETA(DisplayName = "Aiming")
};
