// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani

#pragma once

#include "HitDirection.generated.h"

UENUM(BlueprintType)
enum class EHitDirection : uint8
{
	RIGHT	 UMETA(DisplayName = "Right"),
	LEFT	 UMETA(DisplayName = "Left"),
	FRONT	 UMETA(DisplayName = "Front"),
	BACK	 UMETA(DisplayName = "Back"),
};
