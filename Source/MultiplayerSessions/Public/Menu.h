// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Menu.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void MenuSetup(int32 NumberOfPublicConnections = 4, FString MatchOfType = TEXT("FreeForAll"), FString LobbyPath=TEXT("/Game/ThirdPerson/Maps/Lobby?listen"));

protected:
	virtual bool Initialize() override;
	virtual void NativeDestruct() override;

private:
	UPROPERTY(meta = (BindWidget))
	class UButton* HostButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* JoinButton;

	UFUNCTION()
	void HostButtonClicked();
	
	UFUNCTION()
	void JoinButtonClicked();

	void MenuTearDown();

	UFUNCTION()
	void OnCreateSession(bool bWasSucessful);

	void OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSucessful);
	void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);
	
	UFUNCTION()
	void OnDestroySession(bool bWasSucessful);
	UFUNCTION()
	void OnStartSession(bool bWasSucessful);

	class UMultiplayerSessionsSubsystem* MultiplayerSessionSubsystem;


	int32 NumPublicConnections{ 4 };
	FString MatchType{ TEXT("FreeForAll") };

	FString LobbyPath{ TEXT("/Game/ThirdPerson/Maps/Lobby") };
};
