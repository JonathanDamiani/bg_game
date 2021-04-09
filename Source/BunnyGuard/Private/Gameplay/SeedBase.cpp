// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani


#include "Gameplay/SeedBase.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Characters/Player/PlayerBase.h"
#include "Gameplay/EnemySpawner.h"
#include "BGGameModeBase.h"
#include "AkGameplayStatics.h"
#include "AkAudioEvent.h"
#include "Kismet/GameplayStatics.h"
#include "BGGameState.h"
#include "AkComponent.h"

// Sets default values
ASeedBase::ASeedBase()
{
	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComponent"));
	RootComponent = CollisionComponent;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	AkAudioComponent = CreateDefaultSubobject<UAkComponent>(TEXT("AkAudioComponent"));
	AkAudioComponent->SetupAttachment(MeshComponent);

	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

	bIsSeedFullActive = false;

	TimeToStartSeedActivation = 2.0f;
}

void ASeedBase::StartSeedActivation_Implementation()
{
	if (!bIsSeedActive)
	{
		bIsSeedActive = true;

		if (bCanUpdateQuestLogOnStartActivate)
		{
			UpdateQuestLog(QuestTextAfterStartActive);
		}

		UWorld* World = GetWorld();

		MeshComponent->SetStaticMesh(UpdatingMesh);

		if (SpawnersOfSeed.Num() > 0)
		{
			for (AEnemySpawner* Spawner : SpawnersOfSeed)
			{
				if (Spawner != nullptr)
				{
					Spawner->OnSpawnerDone.AddDynamic(this, &ASeedBase::OnSpawnerFinished);
					Spawner->Spawn(this);
				}
			}
		}
	}
}

void ASeedBase::CompleteActivation_Implementation()
{
	bIsSeedFullActive = true;

	OnSeedActive.Broadcast();

	MeshComponent->SetStaticMesh(FinalMesh);

	if (CompletionEvent != nullptr)
	{
		FOnAkPostEventCallback NullCallback;
		TArray<FAkExternalSourceInfo> External;
		AkAudioComponent->PostAkEvent(CompletionEvent, int32(0), NullCallback, External, CompletionEvent->GetName());
	}

	if (bCanUpdateQuestLogOnEndActivate)
	{
		UpdateQuestLog(QuestTextAfterEndActive);
	}

	UpdatePlayerHUDSeed();
}

void ASeedBase::UpdateQuestLog_Implementation(const FString& QuestText)
{
	UWorld* World = GetWorld();

	if (World != nullptr)
	{
		ABGGameState* GS = Cast<ABGGameState>(UGameplayStatics::GetGameState(World));

		if (GS != nullptr)
		{
			GS->UpdateAllPlayersQuestInfo(QuestText);
		}
	}
	
}

void ASeedBase::UpdatePlayerHUDSeed_Implementation()
{
	UWorld* World = GetWorld();

	if (World != nullptr)
	{
		ABGGameState* GS = Cast<ABGGameState>(UGameplayStatics::GetGameState(World));

		if (GS != nullptr)
		{
			GS->UpdateSeedTreeUI();
		}
	}
}

void ASeedBase::OnInteract_Implementation(AActor* Caller)
{
	if (bCanInteract == false)
	{
		return;
	}

	bCanInteract = false;

	APlayerBase* PlayerInteraction = Cast<APlayerBase>(Caller);

	if (PlayerInteraction != nullptr)
	{
		PlayerInteraction->PlantSeedAction();
	}

	if (ActivationEvent != nullptr)
	{
		FOnAkPostEventCallback NullCallback;
		TArray<FAkExternalSourceInfo> External;
		AkAudioComponent->PostAkEvent(ActivationEvent, int32(0), NullCallback, External, ActivationEvent->GetName());
	}

	GetWorldTimerManager().SetTimer(StartSeedActivation_TimerHandle, this, &ASeedBase::StartSeedActivation, TimeToStartSeedActivation, false);
}

void ASeedBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASeedBase, bIsSeedActive);
	DOREPLIFETIME(ASeedBase, bIsSeedFullActive);
	DOREPLIFETIME(ASeedBase, bCanUpdateQuestLogOnStartActivate);
	DOREPLIFETIME(ASeedBase, bCanUpdateQuestLogOnEndActivate);
}

bool ASeedBase::CheckSeedActivity() const
{
	return bIsSeedFullActive;
}

void ASeedBase::OnSpawnerFinished()
{
	FinishedSpawners++;
	if (FinishedSpawners == SpawnersOfSeed.Num())
	{
		CompleteActivation();	
	}
}
