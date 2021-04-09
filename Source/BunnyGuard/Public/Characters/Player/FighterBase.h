// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani

#pragma once

#include "CoreMinimal.h"
#include "Characters/Player/PlayerBase.h"
#include "FighterBase.generated.h"


class USphereComponent;

UCLASS()
class BUNNYGUARD_API AFighterBase : public APlayerBase
{
	GENERATED_BODY()

public:

	AFighterBase(const class FObjectInitializer& ObjectInitializer);

	virtual void ExecuteBasicAttack_Implementation() override;

	virtual void ExecuteChargedAttack_Implementation() override;

	virtual void ExecuteUtility_Implementation() override;

	virtual void ExecuteCombat_Implementation() override;

	virtual void ExecuteUltimate_Implementation() override;
	
	//////////////////////////////////////////////////////////////////////////
	// Basic Attack

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Abilites/Basic Attack")
	float StartDistanceBasicAttackMultiplier;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Abilites/Basic Attack")
	float DistanceToTravelBasicAttackMultiplier;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Abilites/Basic Attack")
	float BasicAttackCollisionRadius;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Abilites/Basic Attack")
	bool bIsShowingDebugBasicAttack;

	//////////////////////////////////////////////////////////////////////////
	// Charged Attack

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Abilites/Charged Attack")
	float StartDistanceChargedAttackMultiplier;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Abilites/Charged Attack")
	float DistanceToTravelChargedAttackMultiplier;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Abilites/Charged Attack")
	float ChargedAttackCollisionRadius;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Abilites/Charged Attack")
	bool bIsShowingDebugChargedAttack;

	//////////////////////////////////////////////////////////////////////////
	// Combat Attack

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Abilites/Combat Attack")
	float StartDistanceCombatAttackMultiplier;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Abilites/Combat Attack")
	float DistanceToTravelCombatAttackMultiplier;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Abilites/Combat Attack")
	float CombatAttackCollisionRadius;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Abilites/Combat Attack")
	bool bIsShowingDebugCombatAttack;

	UFUNCTION(BlueprintCallable)
	void ResetUtilityOnEnd();

	
	//////////////////////////////////////////////////////////////////////////
	// Ultimate Attack

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Abilites/Ultimate Attack")
	float StartDistanceUltimateAttackMultiplier;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Abilites/Ultimate Attack")
	float DistanceToTravelUltimateAttackMultiplier;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Abilites/Ultimate Attack")
	float UltimateAttackCollisionRadius;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Abilites/Ultimate Attack")
	bool bIsShowingDebugUltimateAttack;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Abilites/Ultimate Attack")
	int32 NumberOfHitsUltimate;

	int32 CurrentNumberOfHitsUltimate;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USphereComponent* UtilityCollider;

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnUtilityOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void SendUtilityEventBegin();

	UFUNCTION(Server, Reliable)
	void HandleDamageExecutionForUtility(AActor* ActorToDamage);
	void HandleDamageExecutionForUtility_Implementation(AActor* ActorToDamage);

	FTimerHandle UltimateAttack_TimerHandle;

	void UltimateAttackAction();
};
