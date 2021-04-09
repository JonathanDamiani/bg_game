// Bunny Guard All Rigths.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryTest.h"
#include "EnvQueryTest_PlayerIsAlive.generated.h"

/**
 * 
 */
UCLASS()
class BUNNYGUARD_API UEnvQueryTest_PlayerIsAlive : public UEnvQueryTest
{
	GENERATED_BODY()
	
	UEnvQueryTest_PlayerIsAlive();

	UPROPERTY(EditDefaultsOnly, Category = "Context")
	TSubclassOf<UEnvQueryContext> PlayerIsAliveContext;

	virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;

	virtual FText GetDescriptionTitle() const override;
	virtual FText GetDescriptionDetails() const override;
};
