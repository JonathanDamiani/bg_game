// Copyright (C) 2020 Alejandro G�ereca Valdivia / Jonathan Dean Damiani


#include "BGEngineSubsystem.h"
#include "AbilitySystemGlobals.h"

void UBGEngineSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UAbilitySystemGlobals::Get().InitGlobalData();
}
