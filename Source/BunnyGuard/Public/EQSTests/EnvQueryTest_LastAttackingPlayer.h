// Bunny Guard All Rigths.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryTest.h"
#include "EnvQueryTest_LastAttackingPlayer.generated.h"

/**
 * 
 */
UCLASS()
class BUNNYGUARD_API UEnvQueryTest_LastAttackingPlayer : public UEnvQueryTest
{
	GENERATED_BODY()

	UEnvQueryTest_LastAttackingPlayer();

	/** context */
	UPROPERTY(EditDefaultsOnly, Category = "Last Attacking Player")
	TSubclassOf<UEnvQueryContext> LastAttackingPlayer;

	virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;

	virtual FText GetDescriptionTitle() const override;
	virtual FText GetDescriptionDetails() const override;
};
