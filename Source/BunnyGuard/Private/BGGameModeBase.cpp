// Copyright (C) 2020 Alejandro G�ereca Valdivia / Jonathan Dean Damiani


#include "BGGameModeBase.h"
#include "Characters/Player/PlayerBase.h"
#include "Characters/Player/PlayerStateBase.h"
#include "Characters/Player/PlayerControllerBase.h"
#include "Characters/Enemies/EnemyBase.h"
#include "Gameplay/SeedBase.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/GameSession.h"
#include "EngineUtils.h"
#include "Characters/Enemies/EnemyAIControllerBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BunnyGuardHttpRequestHandler.h"
#include "Kismet/GameplayStatics.h"
#include "BGGameState.h"

void ABGGameModeBase::SwapCharacter(APlayerController* PlayerController, TSubclassOf<APlayerBase> CharacterToSwap)
{
	if (PlayerController != nullptr)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		APlayerBase* OldPlayer = Cast<APlayerBase>(PlayerController->GetPawn());

		UWorld* World = GetWorld();

		if (OldPlayer != nullptr && World != nullptr)
		{
			FVector PositionToSpawn = OldPlayer->GetActorLocation();
			FRotator RotationToSpawn = OldPlayer->GetActorRotation();

			OldPlayer->RemoveCharacterAbilities();

			OldPlayer->Destroy(true);

			APlayerBase* SpawnedPlayer = World->SpawnActor<APlayerBase>(CharacterToSwap, PositionToSpawn, RotationToSpawn, SpawnParameters);

			PlayerController->Possess(SpawnedPlayer);
		}
	}
}

void ABGGameModeBase::SpawnPlayer(APlayerController* PlayerController, TSubclassOf<APlayerBase> PlayerToSpawn, EPlayerCharacterClass EPlayerClass)
{
	if (PlayerController != nullptr)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		APlayerControllerBase* PC = Cast<APlayerControllerBase>(PlayerController);

		if (PC == nullptr)
		{
			return;
		}

		AActor* PlayerStart;
		if (!bIsInTestMode)
		{
			PlayerStart = FindPlayerStart(PlayerController, FString::FromInt((int)EPlayerClass));
		}
		else
		{
			PlayerStart = FindPlayerStart(PlayerController, TestModeStartPosition);
		}
		
		APlayerBase* OldPlayer = Cast<APlayerBase>(PlayerController->GetPawn());
		UWorld* World = GetWorld();

		if (OldPlayer != nullptr)
		{
			OldPlayer->RemoveCharacterAbilities();

			PC->GetPawn()->Destroy(true);
			PC->RemovePlayerHUD();
		}

		if (World != nullptr && PlayerStart != nullptr)
		{
			FVector PositionToSpawn = PlayerStart->GetActorLocation();
			FRotator RotationToSpawn = PlayerStart->GetActorRotation();

			APlayerBase* SpawnedPlayer = World->SpawnActor<APlayerBase>(PlayerToSpawn, PositionToSpawn, RotationToSpawn, SpawnParameters);
			PC->UnPossess();

			PC->Possess(SpawnedPlayer);
			PC->RemovePlayerHUD();
			PC->CreatePlayerHUD();

			APlayerStateBase* PSBase = Cast<APlayerStateBase>(SpawnedPlayer->GetPlayerState());

			if (PSBase != nullptr)
			{
				UBGGameInstance* GI = Cast<UBGGameInstance>(World->GetGameInstance());
				if (GI != nullptr && SpawnedPlayer->TelemetryId < 0)
				{
					BunnyGuardHttpRequestHandler::SendPlayerCreate(SpawnedPlayer->TelemetryId, GI->GameSessionId, PSBase->ThisPlayerInfo.NetPlayerName, static_cast<int8>(EPlayerClass));
				}
			}
		}
	}
}

void ABGGameModeBase::CheckDefeatCondition()
{
	UWorld* World = GetWorld();

	if (World != nullptr)
	{
		for (TActorIterator<APlayerBase> ActorIter(World); ActorIter; ++ActorIter)
		{
			APlayerBase* PlayerToCheck = *ActorIter;

			if (PlayerToCheck != nullptr)
			{
				if (PlayerToCheck->IsAlive())
				{
					return;
				}
			}
		}

		ActiveEndGameScreen(false);
	}
}

void ABGGameModeBase::CheckVictoryCondition()
{
	ActiveEndGameScreen(true);
}

void ABGGameModeBase::ActiveEndGameScreen(bool bIsVictory)
{
	UWorld* World = GetWorld();
	if (World != nullptr)
	{
		UGameplayStatics::SetGlobalTimeDilation(World, 0.4f);
		
		ABGGameState* GS = Cast<ABGGameState>(UGameplayStatics::GetGameState(World));

		if (GS != nullptr)
		{
			GS->SendEndCondition(bIsVictory);
		}
		
		UBGGameInstance* GI = Cast<UBGGameInstance>(World->GetGameInstance());
		if (GI != nullptr)
		{
			BunnyGuardHttpRequestHandler::SendGameEnd(GI->GameSessionId, bIsVictory);
		}
	}
}

void ABGGameModeBase::EnterCombatState(const bool bIsBoss)
{
	if (bCombatMusicIsPlaying == true)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (World != nullptr)
	{
		for (TActorIterator<APlayerBase> ActorIter(World); ActorIter; ++ActorIter)
		{
			APlayerBase* CurrentPlayer = *ActorIter;
			BunnyGuardHttpRequestHandler::SendCombatStart(CurrentPlayer->TelemetryId);
			CurrentPlayer->ResetDamageTaken();
			CurrentPlayer->PlayCombatMusic(bIsBoss);
		}
	}
	bCombatMusicIsPlaying = true;
}

void ABGGameModeBase::ExitCombatState()
{
	if (bCombatMusicIsPlaying == false)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (World != nullptr)
	{
		bool bEnemyStillTargeting = false;
		for (TActorIterator<AEnemyBase> ActorIter(World); ActorIter; ++ActorIter)
		{
			AEnemyBase* CurrentEnemy = *ActorIter;

			if (IsValid(CurrentEnemy) == false)
			{
				continue;
			}

			if (CurrentEnemy->GetHealth() <= 0)
			{
				continue;
			}

			AEnemyAIControllerBase* EnemyController = Cast<AEnemyAIControllerBase>(CurrentEnemy->GetController());

			if (EnemyController == nullptr)
			{
				continue;
			}
			UBlackboardComponent* EnemyBlackboard = EnemyController->GetBlackboardComponent();

			if (EnemyBlackboard == nullptr)
			{
				continue;
			}
			UObject* PlayerReference = EnemyBlackboard->GetValueAsObject(FName("PlayerReference"));
			
			if (PlayerReference != nullptr)
			{
				bEnemyStillTargeting = true;
				return;
			}
		}

		if (bEnemyStillTargeting == false)
		{
			for (TActorIterator<APlayerBase> ActorIter(World); ActorIter; ++ActorIter)
			{
				APlayerBase* CurrentPlayer = *ActorIter;
				BunnyGuardHttpRequestHandler::SendCombatEnd(CurrentPlayer->TelemetryId, CurrentPlayer->GetDamageTaken());
				CurrentPlayer->StopCombatMusic();
			}
		}
	}

	bCombatMusicIsPlaying = false;
}

void ABGGameModeBase::GenericPlayerInitialization(AController* C)
{
	Super::GenericPlayerInitialization(C);

	APlayerControllerBase* PCBase = Cast<APlayerControllerBase>(C);

	if (PCBase != nullptr)
	{
		PCBase->SetCharacterToSpawn();
	}
}

void ABGGameModeBase::TravelToLevel(FString LevelToTravel)
{
	UWorld* World = GetWorld();

	if (World != nullptr)
	{
		World->ServerTravel("/Game/Maps/Levels/" + LevelToTravel + "?listen");
	}
}

void ABGGameModeBase::RestartLevel()
{
	UWorld* World = GetWorld();

	if (World != nullptr)
	{
		ABGGameState* GS = Cast<ABGGameState>(UGameplayStatics::GetGameState(World));
		if (GS != nullptr)
		{
			GS->ResetPlayersToRestart();
		}

		World->ServerTravel("/Game/Maps/Levels/Level_RealLevel?listen");
	}
}
