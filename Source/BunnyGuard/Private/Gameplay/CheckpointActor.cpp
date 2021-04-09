// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani


#include "Gameplay/CheckpointActor.h"
#include "Components/BoxComponent.h"

// Sets default values
ACheckpointActor::ACheckpointActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	bAlwaysRelevant = true;

	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider"));
	BoxCollider->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}