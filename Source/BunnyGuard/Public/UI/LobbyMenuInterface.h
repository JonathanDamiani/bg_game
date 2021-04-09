// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerInfo.h"
#include "LobbyMenuInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class ULobbyMenuInterface : public UInterface
{
	GENERATED_BODY()
};

class BUNNYGUARD_API ILobbyMenuInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void ChooseCharacter(EPlayerCharacterClass EPlayerSelected) = 0;
};
