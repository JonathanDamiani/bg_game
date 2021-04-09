// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/EngineTypes.h"
#include "Gameplay/InteractionInterface.h"
#include "Net/UnrealNetwork.h"
#include "SeedBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSeedActivatedDel);

class UStaticMeshComponent;
class UBoxComponent;
class AEnemySpawner;
class UAkAudioEvent;
class UAkComponent;

UCLASS()
class BUNNYGUARD_API ASeedBase : public AActor, public IInteractionInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASeedBase();

	UPROPERTY(BlueprintAssignable)
	FSeedActivatedDel OnSeedActive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Seed Defaults")
	UStaticMesh* UpdatingMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Seed Defaults")
	UStaticMesh* FinalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* CollisionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TimeToStartSeedActivation;

	UPROPERTY(EditAnywhere, Category = "Seed Defaults")
	UMaterial* OnStartActiveMaterial;

	UPROPERTY(EditAnywhere, Category = "Seed Defaults")
	UMaterial* OnEndActiveMaterial;

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void StartSeedActivation();
	void StartSeedActivation_Implementation();

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void CompleteActivation();
	void CompleteActivation_Implementation();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void UpdateQuestLog(const FString& QuestText);
	void UpdateQuestLog_Implementation(const FString& QuestText);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void UpdatePlayerHUDSeed();
	void UpdatePlayerHUDSeed_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Interact")
	void OnInteract(AActor* Caller);
	virtual void OnInteract_Implementation(AActor* Caller);

	void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty> & OutLifetimeProps) const override;

	bool CheckSeedActivity() const;

	UFUNCTION()
	void OnSpawnerFinished();

protected:

	UPROPERTY(BlueprintReadWrite, Replicated)
	bool bIsSeedActive = false;

	UPROPERTY(BlueprintReadWrite, Replicated)
	bool bIsSeedFullActive = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	bool bCanUpdateQuestLogOnStartActivate = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	bool bCanUpdateQuestLogOnEndActivate = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Sound)
	UAkComponent* AkAudioComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString QuestTextAfterStartActive;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString QuestTextAfterEndActive;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<AEnemySpawner*> SpawnersOfSeed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Sound)
	UAkAudioEvent* ActivationEvent = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Sound)
	UAkAudioEvent* CompletionEvent = nullptr;

	FTimerHandle StartSeedActivation_TimerHandle;

	int32 FinishedSpawners = 0;

	bool bCanInteract = true;
};
