// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani

#include "Characters/Player/PlayerStateBase.h"
#include "AbilitySystem/BGAbilitySystemComponent.h"
#include "AbilitySystem/AttributeSetBase.h"
#include "Characters/Player/PlayerControllerBase.h"
#include "Characters/Player/LobbyPlayerController.h"
#include "Characters/Player/PlayerBase.h"
#include "BGGameInstance.h"
#include "UI/PlayerHUDBase.h"
#include "Kismet/GameplayStatics.h"
#include "BGGameModeBase.h"
#include "BGGameState.h"

APlayerStateBase::APlayerStateBase()
{
	AbilitySystemComponent = CreateDefaultSubobject<UBGAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	// FULL: Replicate GameplayEffect to everyone
	// MIXED: Replicate GameplayEffect to client only. CUE and Tags to everyone
	// Minimal: CUE and Tags to everyone. Gameplay Effect to none. <---- AI
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSetBase = CreateDefaultSubobject<UAttributeSetBase>(TEXT("AttributeSetBase"));

	NetUpdateFrequency = 50.0f;

	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
	DisableAbilitiesTag = FGameplayTag::RequestGameplayTag(FName("State.DisableAbilities"));
}

UAbilitySystemComponent* APlayerStateBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAttributeSetBase* APlayerStateBase::GetAttributeSet() const
{
	return AttributeSetBase;
}

bool APlayerStateBase::IsAlive() const
{
	return GetHealth() > 0.0f;
}

float APlayerStateBase::GetHealth() const
{
	return AttributeSetBase->GetHealth();
}

float APlayerStateBase::GetMaxHealth() const
{
	return AttributeSetBase->GetMaxHealth();
}

float APlayerStateBase::GetHealthRegenRate() const
{
	return AttributeSetBase->GetHealthRegenRate();
}

float APlayerStateBase::GetMoveSpeed() const
{
	return AttributeSetBase->GetMoveSpeed();
}

void APlayerStateBase::SetPlayerInfo()
{
	UBGGameInstance* GI = Cast<UBGGameInstance>(GetGameInstance());

	if (GI != nullptr)
	{
		ThisPlayerInfo = GI->LocalPlayerInfo;
	}
}

void APlayerStateBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerStateBase, ClassSelected);
	DOREPLIFETIME(APlayerStateBase, ThisPlayerInfo);
}

void APlayerStateBase::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthAttribute()).AddUObject(this, &APlayerStateBase::HealthChanged);
		MaxHealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetMaxHealthAttribute()).AddUObject(this, &APlayerStateBase::MaxHealthChanged);
		HealthRegenRateChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthRegenRateAttribute()).AddUObject(this, &APlayerStateBase::HealthRegenRateChanged);
	}
}

void APlayerStateBase::HealthChanged(const FOnAttributeChangeData& Data)
{
	float Health = Data.NewValue;

	APlayerBase* Player = Cast<APlayerBase>(GetPawn());

	APlayerControllerBase* PC = Cast<APlayerControllerBase>(GetOwner());

	const float DeltaHealth = Data.OldValue - Data.NewValue;

	if (Player != nullptr && DeltaHealth > 0)
	{
		Player->AddDamageTaken(DeltaHealth);
	}

	if (PC)
	{
		UPlayerHUDBase* HUD = PC->GetPlayerHUD();

		if (HUD)
		{
			HUD->SetHealthPercentage(Health / GetMaxHealth());
		}

	}

	if (!IsAlive() && !AbilitySystemComponent->HasMatchingGameplayTag(DeadTag))
	{
		Player->Die();
	}

}

void APlayerStateBase::MaxHealthChanged(const FOnAttributeChangeData& Data)
{
	float MaxHealth = Data.NewValue;

	APlayerControllerBase* PC = Cast<APlayerControllerBase>(GetOwner());

	if (PC)
	{
		UPlayerHUDBase* HUD = PC->GetPlayerHUD();

		if (HUD)
		{
			HUD->SetMaxHealth(MaxHealth);
		}
	}
}

void APlayerStateBase::HealthRegenRateChanged(const FOnAttributeChangeData & Data)
{
	float HealthRegenRate = Data.NewValue;
	// Change to health Regen
}
