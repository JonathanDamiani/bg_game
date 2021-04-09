// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerInfo.h"
#include "Styling/SlateColor.h"
#include "PlayerInfoItemWidget.generated.h"

class UTextBlock;


UCLASS()
class BUNNYGUARD_API UPlayerInfoItemWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* PlayerName_Text;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PlayerClass_Text;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PlayerReady_Text;

	UFUNCTION(BlueprintCallable)
	void SetPlayerNameUI(FText playerName);

	UFUNCTION(BlueprintCallable)
	void SetPlayerClassUI(EPlayerCharacterClass EPlayerClass);

	UFUNCTION(BlueprintCallable)
	void SetPlayerReady(bool bIsPlayerReady);

	UFUNCTION(BlueprintCallable)
	void SetAllPlayerInfo(FPlayerInfo AllInfo);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FSlateColor TextReadyColor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FSlateColor TextNotReadyColor;
};
