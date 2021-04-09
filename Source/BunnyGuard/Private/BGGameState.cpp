// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani


#include "BGGameState.h"
#include "Characters/Player/PlayerControllerBase.h"
#include "Characters/Player/PlayerBase.h"
#include "GameFramework/PlayerState.h"

void ABGGameState::SetAllPlayersHUDInfo_Implementation()
{
	for (APlayerState* PS : PlayerArray)
	{
		if (PS != nullptr)
		{
			APlayerControllerBase* PC = Cast<APlayerControllerBase>(PS->GetOwner());
			if (PC != nullptr)
			{
				PC->SetPlayersInfoEvent();
			}
		}
	}
}

void ABGGameState::UpdateAllPlayersHUDInfo_Implementation()
{
	for (APlayerState* PS : PlayerArray)
	{
		if (PS != nullptr)
		{
			APlayerControllerBase* PC = Cast<APlayerControllerBase>(PS->GetOwner());
			if (PC != nullptr)
			{
				PC->UpdatePlayersInfoEvent();
			}
		}
	}
}

void ABGGameState::UpdateAllPlayersQuestInfo_Implementation(const FString& QuestText)
{
	for (APlayerState* PS : PlayerArray)
	{
		if (PS != nullptr)
		{
			APlayerControllerBase* PC = Cast<APlayerControllerBase>(PS->GetOwner());
			if (PC != nullptr)
			{
				PC->UpdatePlayerQuestEvent(QuestText);
			}
		}
	}
}

void ABGGameState::UpdateBossHealth_Implementation(float BossHealth)
{
	for (APlayerState* PS : PlayerArray)
	{
		if (PS != nullptr)
		{
			APlayerControllerBase* PC = Cast<APlayerControllerBase>(PS->GetOwner());
			if (PC != nullptr)
			{
				PC->UpdateBossHealth(BossHealth);
			}
		}
	}
}

void ABGGameState::ResetPlayersToRestart_Implementation()
{
	for (APlayerState* PS : PlayerArray)
	{
		if (PS != nullptr)
		{
			APlayerControllerBase* PC = Cast<APlayerControllerBase>(PS->GetOwner());
			if (PC != nullptr)
			{
				PC->RemoveEndScreen();
				PC->DetachTheCamera();
				PC->SetCharacterToSpawn();
			}
		}
	}
}

void ABGGameState::UpdateSeedTreeUI_Implementation()
{
	for (APlayerState* PS : PlayerArray)
	{
		if (PS != nullptr)
		{
			APlayerControllerBase* PC = Cast<APlayerControllerBase>(PS->GetOwner());
			if (PC != nullptr)
			{

				PC->SeedActivationHUD();
			}
		}
	}
}


void ABGGameState::SendEndCondition_Implementation(bool bIsVictory)
{
	for (APlayerState* PS : PlayerArray)
	{
		if (PS != nullptr)
		{
			APlayerControllerBase* PC = Cast<APlayerControllerBase>(PS->GetOwner());
			if (PC != nullptr)
			{
				PC->CreateEndGameScreen(bIsVictory);
			}
		}
	}
}
void ABGGameState::TeleportPlayersToPosition_Implementation(const TArray<FTransform>& TransformToTeleport)
{	
	int counter = 0;

	for (APlayerState* PS : PlayerArray)
	{
		if (PS != nullptr)
		{
			APlayerBase* PlayerBase = Cast<APlayerBase>(PS->GetPawn());

			if (PlayerBase != nullptr)
			{
				PlayerBase->TeleportTo(TransformToTeleport[counter].GetLocation(), TransformToTeleport[counter].Rotator());

				counter++;
			}
		}
	}
}
