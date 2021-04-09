// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani

#pragma once

#include "CoreMinimal.h"
#include "Characters/Player/PlayerBase.h"
#include "PlayerInfo.generated.h"

UENUM(BlueprintType)
enum class EPlayerCharacterClass : uint8
{
	None,
	// Knight
	Knight,
	// Fighter
	Fighter,
	// Ranger
	Ranger,
	// Mage
	Mage
};

USTRUCT(BlueprintType)
struct FPlayerInfo
{
	GENERATED_BODY()

public:
	UPROPERTY()
	int32 id = -1;

	UPROPERTY(BlueprintReadWrite)
	FText NetPlayerName = FText::FromString("");

	UPROPERTY(BlueprintReadWrite)
	bool IsPlayerReady = false;

	UPROPERTY(BlueprintReadWrite)
	EPlayerCharacterClass NetPlayerClass = EPlayerCharacterClass::None;

	UPROPERTY()
	int8 NetPlayerIndex = -1;

};
