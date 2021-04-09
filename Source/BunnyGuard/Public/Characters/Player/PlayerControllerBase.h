// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerInfo.h"
#include "Net/UnrealNetwork.h"
#include "PlayerControllerBase.generated.h"

class UPlayerHUDBase;
class UOptionsMenuBase;
class APlayerBase;
class UEndGameScreenBase;

UCLASS()
class BUNNYGUARD_API APlayerControllerBase : public APlayerController
{
	GENERATED_BODY()

public:

	UFUNCTION()
	void CreatePlayerHUD();

	UFUNCTION()
	void RemovePlayerHUD();
	
	void CreateOptionMenu();

	UFUNCTION()
	void OpenOptions();

	UFUNCTION()
	void CloseOptions();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetPlayersInfoEvent();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdatePlayersInfoEvent();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdatePlayerQuestEvent(const FString& QuestText);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateBossHealth(float BossHealth);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void EnableInteractText(AActor* InteractableActor);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void DisableInteractText();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SeedActivationHUD();

	UFUNCTION(Server, Unreliable, WithValidation, BlueprintCallable)
	void UpdateAllPlayerInfoServer();
	void UpdateAllPlayerInfoServer_Implementation();
	bool UpdateAllPlayerInfoServer_Validate();

	UFUNCTION(BlueprintCallable)
	void ToogleOptionsMenu();

	UFUNCTION(BlueprintCallable)
	virtual UPlayerHUDBase* GetPlayerHUD();

	UFUNCTION(BlueprintCallable)
	void CreateEndGameScreen(bool IsVictory);

	UFUNCTION(BlueprintCallable)
	void RemoveEndScreen();

	UFUNCTION(BlueprintCallable)
	void DetachTheCamera();

	UFUNCTION(Client, Reliable)
	void SetCharacterToSpawn();
	void SetCharacterToSpawn_Implementation();

	UFUNCTION(Server, Reliable)
	void CallPlayerSpawn(TSubclassOf<APlayerBase> CharacterSelected,EPlayerCharacterClass EPlayerClass);
	void CallPlayerSpawn_Implementation(TSubclassOf<APlayerBase> CharacterSelected, EPlayerCharacterClass EPlayerClass);

	UFUNCTION(BlueprintCallable)
	FString GetPlayerNameByClass();

	UFUNCTION(BlueprintCallable)
	void PlayAbilityFailToActiveHUD(FGameplayTagContainer InTags);

	void AttachCamera();
protected:

	virtual void OnPossess(APawn* pawn) override;
	virtual void OnUnPossess() override;
	virtual void OnRep_PlayerState() override;

	//////////////////////////////////////////////////////////////////////////
	// Player HUD and Menu

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
	TSubclassOf<UPlayerHUDBase> PlayerHUDClass;

	UPROPERTY(BlueprintReadWrite, Category = "UI")
	UPlayerHUDBase* PlayerHUD;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
	TSubclassOf<UOptionsMenuBase> PlayerOptionsClass;

	UPROPERTY(BlueprintReadWrite, BlueprintReadWrite, Category = "UI")
	UOptionsMenuBase* PlayerOptions;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
	TSubclassOf<UEndGameScreenBase> DefeatScreenClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
	TSubclassOf<UEndGameScreenBase> VictoryScreenClass;

	UPROPERTY(BlueprintReadWrite, Category = "UI")
	UEndGameScreenBase* EndScreen;

	//////////////////////////////////////////////////////////////////////////
	// Character Selection

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Character to choose")
	TSubclassOf<APlayerBase> ClassToSpawn;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Character to choose")
	TSubclassOf<APlayerBase> KnightClass;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Character to choose")
	TSubclassOf<APlayerBase> FighterClass;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Character to choose")
	TSubclassOf<APlayerBase> RangerClass;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Character to choose")
	TSubclassOf<APlayerBase> MageClass;

	bool bIsOptionsOpen = false;

	virtual void BeginPlay() override;
};
