// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani

#pragma once

#include "CoreMinimal.h"
#include "Characters/Player/PlayerBase.h"
#include "KnightBase.generated.h"

class USkeletalMeshComponent;
class USkeletalMesh;

UCLASS()
class BUNNYGUARD_API AKnightBase : public APlayerBase
{
	GENERATED_BODY()
public:
	
	AKnightBase(const class FObjectInitializer& ObjectInitializer);

	virtual void ExecuteBasicAttack_Implementation() override;

	virtual void ExecuteChargedAttack_Implementation() override;

	virtual void ExecuteUtility_Implementation() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USkeletalMeshComponent* SwordMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USkeletalMeshComponent* ShieldMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMesh* MainSwordMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMesh* MainShieldMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMesh* MegaSwordMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMesh* MegaShieldMesh;

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
	// Utility Skill

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Abilites/Utility Skill")
	int32 CurrentNumberOfHits;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Abilites/Utility Skill")
	int32 MaxNumberOfHits;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Abilites/Utility Skill")
	int32 DamageLevel;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Abilites/Utility Skill")
	float TimeToResetDamage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Abilites/Utility Skill")
	float TimeToResetHit;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Abilites/Utility Skill")
	bool CanPlayerBlock;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Abilites/Utility Skill")
	bool IsPlayerBlocking;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Abilites/Utility Skill")
	bool IsMegaMode;

	FTimerHandle ResetBlock_TimerHandle;

	FTimerHandle ResetHit_TimerHandle;

	UFUNCTION(BlueprintCallable)
	void ExecuteResetBlock();

	UFUNCTION(BlueprintCallable)
	void StartResetHit();

	UFUNCTION(BlueprintCallable)
	void StopResetHit();

	UFUNCTION(BlueprintCallable)
	void HandleResetHit();

	UFUNCTION(BlueprintCallable, NetMulticast, Unreliable)
	void SwapMeshes(USkeletalMesh* Shield, USkeletalMesh* Sword);
	void SwapMeshes_Implementation(USkeletalMesh* Shield, USkeletalMesh* Sword);
};
