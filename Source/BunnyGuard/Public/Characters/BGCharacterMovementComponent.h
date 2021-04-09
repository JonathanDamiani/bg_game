// Bunny Guard All Rigths.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BGCharacterMovementComponent.generated.h"

UCLASS()
class BUNNYGUARD_API UBGCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
		class FGDSavedMove : public FSavedMove_Character
	{
	public:

		typedef FSavedMove_Character Super;

		virtual void Clear() override;

		virtual uint8 GetCompressedFlags() const override;

		virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const override;

		virtual void SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character & ClientData) override;

		virtual void PrepMoveFor(class ACharacter* Character) override;

		uint8 SavedRequestToStartSprinting : 1;

		uint8 SavedRequestToStartADS : 1;
	};

	class FGDNetworkPredictionData_Client : public FNetworkPredictionData_Client_Character
	{
	public:
		FGDNetworkPredictionData_Client(const UCharacterMovementComponent& ClientMovement);

		typedef FNetworkPredictionData_Client_Character Super;

		virtual FSavedMovePtr AllocateNewMove() override;
	};

public:
	UBGCharacterMovementComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprint")
	float SprintSpeedMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Down Sights")
	float ADSSpeedMultiplier;

	uint8 RequestToStartSprinting : 1;
	uint8 RequestToStartADS : 1;

	virtual float GetMaxSpeed() const override;
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	virtual class FNetworkPredictionData_Client* GetPredictionData_Client() const override;

	UFUNCTION(BlueprintCallable, Category = "Sprint")
	void StartSprinting();
	UFUNCTION(BlueprintCallable, Category = "Sprint")
	void StopSprinting();

	// Aim 
	UFUNCTION(BlueprintCallable, Category = "Aim Down Sights")
	void StartAimDownSights();
	UFUNCTION(BlueprintCallable, Category = "Aim Down Sights")
	void StopAimDownSights();
};
