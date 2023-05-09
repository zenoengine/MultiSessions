// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MultiplayerSessionsSubsystem.generated.h"



DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnCreateSessionComplete, bool, bWasSucessful);
DECLARE_MULTICAST_DELEGATE_TwoParams(FMultiplayerOnFindSesionsComplete, const TArray<FOnlineSessionSearchResult>& Session, bool bWasSucessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FMultiplayerOnJoinSessionComplete, EOnJoinSessionCompleteResult::Type Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnDestroySessionComplete, bool, bWasSucessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnStartSessionComplete, bool, bWasSucessful);

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMultiplayerSessionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UMultiplayerSessionsSubsystem();

	void CreateSession(int32 NumPublicConnections, FString MatchType);
	void FindSessions(int32 MaxSearchResults);
	void JoinSession(const FOnlineSessionSearchResult& SessionResult);
	void DestroySession();
	void StartSession();

	FMultiplayerOnCreateSessionComplete OnCreateSessionCompleteDelegate;
	FMultiplayerOnFindSesionsComplete OnFindSessionsCompleteDelegate;
	FMultiplayerOnJoinSessionComplete OnJoinSessionCompleteDelegate;
	FMultiplayerOnDestroySessionComplete OnDestroySessionCompleteDelegate;
	FMultiplayerOnStartSessionComplete OnStartSessionCompleteDelegate;

protected:
	void OnCreateSessionComplete(FName SessionName, bool bWasSucessful);
	void OnFindSessionsComplete(bool bWasSucessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Type);
	void OnDestroySessionComplete(FName SessionName, bool bWasSucessful);
	void OnStartSessionComplete(FName SessionName, bool bWasSucessful);

private:

	bool IsLanMatch();

private:
	IOnlineSessionPtr SessionInterface;
	TSharedPtr<FOnlineSessionSettings> LastSessionSettings;
	TSharedPtr<FOnlineSessionSearch> LastSessionSearch;
	bool bCreateSessionBlockedByDestroySession{false};

	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	FDelegateHandle CreateSessionCompleteDelegateHandle;
	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
	FDelegateHandle FindSessionsCompleteDelegateHandle;
	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
	FDelegateHandle JoinSessionCompleteDelegateHandle;
	FOnDestroySessionCompleteDelegate DestroySessionCompleteDelegate;	
	FDelegateHandle DestroySessionCompleteDelegateHandle;
	FOnStartSessionCompleteDelegate StartSessionCompleteDelegate;
	FDelegateHandle StartSessionCompleteDelegateHandle;
};
