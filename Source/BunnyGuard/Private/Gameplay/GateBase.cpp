// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani


#include "Gameplay/GateBase.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Gameplay/SeedBase.h"
#include "AkComponent.h"
#include "AkGameplayStatics.h"
#include "AkAudioEvent.h"


// Sets default values
AGateBase::AGateBase()
{
	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComponent"));
	RootComponent = CollisionComponent;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	AudioComponent = CreateDefaultSubobject<UAkComponent>(TEXT("AudioComponent"));
	AudioComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	bAlwaysRelevant = true;
}

void AGateBase::OpenGate()
{	
	
	for (ASeedBase* Seed : SeedsOfGate)
	{
		if (!Seed->CheckSeedActivity())
		{
			return;
		}
	}

	OnDestroy();
}

void AGateBase::OnDestroy_Implementation()
{
	return;
};

void AGateBase::BeginPlay()
{
	Super::BeginPlay();

	for (ASeedBase* Seed : SeedsOfGate)
	{
		Seed->OnSeedActive.AddDynamic(this, &AGateBase::OpenGate);
	}

	FOnAkPostEventCallback NullCallback;
	TArray<FAkExternalSourceInfo> External;
	if (IsValid(AudioComponent) ==  true)
	{
		AudioComponent->PostAssociatedAkEvent(int32(0), NullCallback, External);
	}
}
