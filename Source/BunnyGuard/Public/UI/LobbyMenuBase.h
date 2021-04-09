// Copyright (C) 2020 Alejandro Güereca Valdivia / Jonathan Dean Damiani

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/LobbyMenuInterface.h"
#include "PlayerInfo.h"
#include "Net/UnrealNetwork.h"
#include "LobbyMenuBase.generated.h"

class UWidgetSwitcher;
class UVerticalBox;
class UPlayerInfoItemWidget;
class UButtonBase;

UCLASS()
class BUNNYGUARD_API ULobbyMenuBase : public UUserWidget
{
	GENERATED_BODY()


protected:
	virtual bool Initialize() override;

	ILobbyMenuInterface* LobbyMenuInterface;
	
	UFUNCTION(BlueprintCallable)
	void SelectCharacter(EPlayerCharacterClass ECharaterSelected);

	UFUNCTION(BlueprintCallable)
	void UpdateAllSelectedCharacters(TArray<FPlayerInfo> PlayersInfo);

	UFUNCTION()
	void UpdateButtonsActive(UButtonBase* ButtonToIgnore);

	UFUNCTION(BlueprintCallable)
	bool CheckPlayerReady(TArray<FPlayerInfo> PlayersInfo);

	UFUNCTION(BlueprintCallable)
	void ActivePlayerReady();
	
	UPROPERTY(meta = (BindWidget))
	UButtonBase* Knight_Button;

	UPROPERTY(meta = (BindWidget))
	UButtonBase* Fighter_Button;

	UPROPERTY(meta = (BindWidget))
	UButtonBase* Mage_Button;

	UPROPERTY(meta = (BindWidget))
	UButtonBase* Ranger_Button;
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (BindWidget))
	UButtonBase* IsReady_Button;

	UPROPERTY(BlueprintReadOnly)
	TArray<UButtonBase*> CharacterSelectionButtons; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPlayerCharacterClass EThisSelectedCharacter;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<EPlayerCharacterClass, UButtonBase*> ButtonsMaps;

public:
	void SetLobbyInterface(ILobbyMenuInterface* InterfaceToSet);
};
