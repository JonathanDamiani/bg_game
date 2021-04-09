// Copyright (C) 2020 Alejandro G�ereca Valdivia / Jonathan Dean Damiani

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "UI/MainMenuInterface.h"
#include "UI/LobbyMenuInterface.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "AllServeInfo.h"
#include "PlayerInfo.h"
#include "BGGameInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FServeDelegate, FAllServeInfo, AllServeInfoDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FServerSearchDelegate, bool, IsSearchingServer);

class UUserWidget;
class UWidget;
class APlayerBase;
class UMainMenuBase;

UCLASS()
class BUNNYGUARD_API UBGGameInstance : public UGameInstance, public IMainMenuInterface
{
	GENERATED_BODY()
	
public:
	virtual void Init() override;

	UFUNCTION(BlueprintCallable)
	void LoadMenu();

	UFUNCTION()
	void Host() override;

	UFUNCTION(BlueprintCallable)
	void EndSession();

	UFUNCTION(BlueprintCallable)
	void JoinServer(int32 ServArrayIndex);

	UFUNCTION()
	void SetServerName(FString CurrentServerName) override;

	UFUNCTION(BlueprintCallable)
	void OpenSessionListMenu() override;

	UFUNCTION(BlueprintCallable)
	bool CheckIfSteamIsOpen();

	UFUNCTION(BlueprintCallable)
	UMainMenuBase* GetMainMenu();

	void SetPlayerGameInputMode();
	void SetPlayerUIInputMode(UWidget* UserWidget);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FPlayerInfo LocalPlayerInfo;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FAllServeInfo ServerInfo;

	UPROPERTY(Replicated)
	int32 GameSessionId = -1;

	void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty> & OutLifetimeProps) const override;

protected:
	
	//////////////////////////////////////////////////////////////////////////
	// Menus Widgets

	UPROPERTY(EditAnywhere, Category="MainMenuWidget")
	TSubclassOf<UUserWidget> MainMenuUserWidget;

	//////////////////////////////////////////////////////////////////////////
	// Online System 

	FString DesiredServerName;

	FName BGSessionName = FName("Game Session");

	IOnlineSessionPtr SessionInterface;

	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	UMainMenuBase* MainMenu;

	UPROPERTY(BlueprintAssignable)
	FServeDelegate AllServeInfoDelegate;

	UPROPERTY(BlueprintAssignable)
	FServerSearchDelegate SearchingServersDelegate;

	void OnCreateSessionComplete(FName SessionName, bool Success);
	void OnDestroySessionComplete(FName SessionName, bool Success);
	void OnFindSessionsComplete(bool Success);
	void OnJoinSessionsComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	bool CheckIfIsLan() const;
};
