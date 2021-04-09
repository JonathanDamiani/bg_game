// Bunny Guard All Rigths.

#include "Characters/Enemies/IncarnateBase.h"
#include "AbilitySystem/BGAbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "BGGameModeBase.h"
#include "BGGameState.h"

AIncarnateBase::AIncarnateBase(const class FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	StatusEffectCancelingTag = FGameplayTag::RequestGameplayTag(FName("State.Invulnerable"));
}

void AIncarnateBase::ActivateBossAbility(EBossAbility AbilityIdentifier)
{
	TSubclassOf<UGameplayAbility> AbilityToActivate = CurrentPhase == EBossPhase::Phase_One ? AbilitiesPhasesOne[AbilityIdentifier] : AbilitiesPhasesTwo[AbilityIdentifier];
	EnemyAbilitySystemComponent->TryActivateAbilityByClass(AbilityToActivate);
}

float AIncarnateBase::GetHealthThreshold()
{
	if (HealthThresholds.Num() > ThresholdIndex)
	{
		return HealthThresholds[ThresholdIndex];
	}

	return 0.0f;
}

void AIncarnateBase::HealthChanged(const FOnAttributeChangeData& Data)
{	
	float Health = Data.NewValue;

	UWorld* World = GetWorld();

	if (World != nullptr)
	{
		ABGGameState* GS = Cast<ABGGameState>(UGameplayStatics::GetGameState(World));
		if (GS != nullptr)
		{
			GS->UpdateBossHealth(Health / GetMaxHealth());
		}
	}

	if (!IsAlive())
	{
		Die();
	}
}

void AIncarnateBase::BeginPlay()
{
	Super::BeginPlay();

	AbilitySystemComponent->AddLooseGameplayTag(StatusEffectCancelingTag);

	float MaxHealthQuarter = GetMaxHealth() / 4;

	HealthThresholds = {
		MaxHealthQuarter * 3,
		MaxHealthQuarter * 2,
		MaxHealthQuarter
	};
}

void AIncarnateBase::Die()
{
	Super::Die();
	UWorld* World = GetWorld();

	if (World != nullptr)
	{
		ABGGameModeBase* GM = Cast<ABGGameModeBase>(UGameplayStatics::GetGameMode(World));

		if (GM != nullptr)
		{	
			GM->CheckVictoryCondition();
		}
	}
}
