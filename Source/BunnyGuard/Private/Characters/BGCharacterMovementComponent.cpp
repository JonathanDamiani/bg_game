// Bunny Guard All Rigths.


#include "Characters/BGCharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "Characters/CharacterBase.h"

UBGCharacterMovementComponent::UBGCharacterMovementComponent()
{
	bOrientRotationToMovement = true;
	RotationRate = FRotator(0.0f, 720.0f, 0.0f);
	AirControl = 0.2f;
	GravityScale = 2.0f;
	JumpZVelocity = 1200.0f;
	ADSSpeedMultiplier = 0.5f;
	SprintSpeedMultiplier = 1.5f;
}

float UBGCharacterMovementComponent::GetMaxSpeed() const
{
	ACharacterBase* Owner = Cast<ACharacterBase>(GetOwner());
	if (!Owner)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() No Owner"), *FString(__FUNCTION__));
		return Super::GetMaxSpeed();
	}

	if (!Owner->IsAlive())
	{
		return 0.0f;
	}

	if (Owner->GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Debuff.Stun"))))
	{
		return 0.0f;
	}

	if (RequestToStartSprinting)
	{
		return Owner->GetMoveSpeed() * SprintSpeedMultiplier;
	}

	if (RequestToStartADS)
	{
		return Owner->GetMoveSpeed() * ADSSpeedMultiplier;
	}

	return Owner->GetMoveSpeed();
}

void UBGCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	RequestToStartSprinting = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;

	RequestToStartADS = (Flags & FSavedMove_Character::FLAG_Custom_1) != 0;
}

class FNetworkPredictionData_Client* UBGCharacterMovementComponent::GetPredictionData_Client() const
{
	check(PawnOwner != NULL);

	if (!ClientPredictionData)
	{
		UBGCharacterMovementComponent* MutableThis = const_cast<UBGCharacterMovementComponent*>(this);

		MutableThis->ClientPredictionData = new FGDNetworkPredictionData_Client(*this);
		MutableThis->ClientPredictionData->MaxSmoothNetUpdateDist = 92.f;
		MutableThis->ClientPredictionData->NoSmoothNetUpdateDist = 140.f;
	}

	return ClientPredictionData;
}

void UBGCharacterMovementComponent::StartSprinting()
{
	RequestToStartSprinting = true;
}

void UBGCharacterMovementComponent::StopSprinting()
{
	RequestToStartSprinting = false;
}

void UBGCharacterMovementComponent::StartAimDownSights()
{
	RequestToStartADS = true;
}

void UBGCharacterMovementComponent::StopAimDownSights()
{
	RequestToStartADS = false;
}

void UBGCharacterMovementComponent::FGDSavedMove::Clear()
{
	Super::Clear();
	SavedRequestToStartSprinting = false;
	SavedRequestToStartADS = false;
}

uint8 UBGCharacterMovementComponent::FGDSavedMove::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();

	if (SavedRequestToStartSprinting)
	{
		Result |= FLAG_Custom_0;
	}

	if (SavedRequestToStartADS)
	{
		Result |= FLAG_Custom_1;
	}

	return Result;
}

bool UBGCharacterMovementComponent::FGDSavedMove::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const
{
	if (SavedRequestToStartSprinting != ((FGDSavedMove*)&NewMove)->SavedRequestToStartSprinting)
	{
		return false;
	}

	if (SavedRequestToStartADS != ((FGDSavedMove*)&NewMove)->SavedRequestToStartADS)
	{
		return false;
	}

	return Super::CanCombineWith(NewMove, Character, MaxDelta);
}

void UBGCharacterMovementComponent::FGDSavedMove::SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character & ClientData)
{
	Super::SetMoveFor(Character, InDeltaTime, NewAccel, ClientData);

	UBGCharacterMovementComponent* CharacterMovement = Cast<UBGCharacterMovementComponent>(Character->GetCharacterMovement());
	if (CharacterMovement)
	{
		SavedRequestToStartSprinting = CharacterMovement->RequestToStartSprinting;
		SavedRequestToStartADS = CharacterMovement->RequestToStartADS;
	}
}

void UBGCharacterMovementComponent::FGDSavedMove::PrepMoveFor(class ACharacter* Character)
{

}

UBGCharacterMovementComponent::FGDNetworkPredictionData_Client::FGDNetworkPredictionData_Client(const UCharacterMovementComponent& ClientMovement)
	: Super(ClientMovement)
{

}

FSavedMovePtr UBGCharacterMovementComponent::FGDNetworkPredictionData_Client::AllocateNewMove()
{
	return FSavedMovePtr(new FGDSavedMove());
}
