// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani


#include "BGEngineSubsystem.h"
#include "AbilitySystemGlobals.h"

void UBGEngineSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UAbilitySystemGlobals::Get().InitGlobalData();
}
