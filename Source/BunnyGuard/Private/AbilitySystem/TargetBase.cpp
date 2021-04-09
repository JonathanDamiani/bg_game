// Bunny Guard All Rigths.


#include "AbilitySystem/TargetBase.h"
#include "Characters/CharacterBase.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ATargetBase::ATargetBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DistanceToSelf = 100.0f;
	MaxDistanceToTarget = 200.0f;
}

// Called when the game starts or when spawned
void ATargetBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATargetBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bEnableReposition)
	{
		DefineActorTransform();
	}
}

void ATargetBase::SetBaseReference(ACharacterBase* CharRef)
{
	BaseCharacterReference = CharRef;
}

void ATargetBase::SetTargetReference(ACharacterBase* TargetRef)
{
	TargetReference = TargetRef;
}

void ATargetBase::EnableReposition()
{
	bEnableReposition = true;
}

void ATargetBase::DefineActorTransform()
{
	if (!IsValid(TargetReference) || !TargetReference->IsValidLowLevel() || !IsValid(BaseCharacterReference) || !BaseCharacterReference->IsValidLowLevel())
	{
		Destroy();
		return;
	}

	if (TargetReference->GetDistanceTo(BaseCharacterReference) >= MaxDistanceToTarget)
	{
		FRotator NewRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), BaseCharacterReference->GetActorLocation());

		SetActorRotation(NewRotation);

		FVector PositionToSet = (TargetReference->GetActorLocation()) + (GetActorForwardVector() * DistanceToSelf);

		SetActorLocation(PositionToSet);
	}
}
