// Bunny Guard All Rigths.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Enemies/EnemyBase.h"
#include "IncarnateBase.generated.h"

UENUM(BlueprintType)
enum class EBossAbility: uint8
{
	Default = 0		  UMETA(DisplayName = "Default"),
	Short_Range = 1   UMETA(DisplayName = "Short_Range"),
	Medium_Range = 2  UMETA(DisplayName = "Medium_Range"),
	Large_Range = 3	  UMETA(DisplayName = "Large_Range"),
	Spawn_Enemies = 4 UMETA(DisplayName = "Spawn_Enemies")
};

UENUM(BlueprintType)
enum class EBossPhase : uint8
{
	Phase_One = 0	 UMETA(DisplayName = "Phase_One"),
	Phase_Two = 1	 UMETA(DisplayName = "Phase_Two"),
};

UCLASS()
class BUNNYGUARD_API AIncarnateBase : public AEnemyBase
{
	GENERATED_BODY()

public:
	FGameplayTag StatusEffectCancelingTag;

	AIncarnateBase(const class FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable)
	void ActivateBossAbility(EBossAbility AbilityToActivate);

	UFUNCTION(BlueprintCallable)
	float GetHealthThreshold();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Abilities")
	TMap<EBossAbility, TSubclassOf<UGameplayAbility>> AbilitiesPhasesOne;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Abilities")
	TMap<EBossAbility, TSubclassOf<UGameplayAbility>> AbilitiesPhasesTwo;

	UPROPERTY(BlueprintReadWrite, Category = "Variables")
	int32 ThresholdIndex = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Variables")
	int32 PhaseChangeThresholdIndex = 2;

	UPROPERTY(BlueprintReadWrite, Category = "Variables")
	EBossPhase CurrentPhase = EBossPhase::Phase_One;

protected:
	virtual void HealthChanged(const FOnAttributeChangeData& Data);

	virtual void Die() override;

private:
	virtual void BeginPlay() override;

	TArray<float> HealthThresholds;
};
