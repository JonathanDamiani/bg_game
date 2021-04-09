// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MainMenuInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UMainMenuInterface : public UInterface
{
	GENERATED_BODY()
};

class BUNNYGUARD_API IMainMenuInterface
{
	GENERATED_BODY()

public:
	virtual void Host() = 0;
	virtual void OpenSessionListMenu() = 0;
	virtual void SetServerName(FString serverName) = 0;
};
