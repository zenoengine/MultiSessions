// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"
#include "Components/Button.h"
#include "OnlineSessionSettings.h"
#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"

void UMenu::MenuSetup(int32 NumberOfPublicConnections, FString MatchOfType, FString LobbyMapPath)
{
	NumPublicConnections = NumberOfPublicConnections;
	MatchType = MatchOfType;
	LobbyPath = LobbyMapPath;
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;
	
	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			FInputModeUIOnly InputModeData;
			InputModeData.SetWidgetToFocus(TakeWidget());
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(true);
		}
	}

	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		MultiplayerSessionSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();

	}
	if (MultiplayerSessionSubsystem)
	{
		MultiplayerSessionSubsystem->OnCreateSessionCompleteDelegate.AddDynamic(this, &ThisClass::OnCreateSession);
		MultiplayerSessionSubsystem->OnFindSessionsCompleteDelegate.AddUObject(this, &ThisClass::OnFindSessions);
		MultiplayerSessionSubsystem->OnJoinSessionCompleteDelegate.AddUObject(this, &ThisClass::OnJoinSession);
		MultiplayerSessionSubsystem->OnDestroySessionCompleteDelegate.AddDynamic(this, &ThisClass::OnDestroySession);
		MultiplayerSessionSubsystem->OnStartSessionCompleteDelegate.AddDynamic(this, &ThisClass::OnStartSession);
	}
}

bool UMenu::Initialize()
{
	if (!Super::Initialize()) 
	{
		return false;
	}

	if (HostButton)
	{
		HostButton->OnClicked.AddDynamic(this, &ThisClass::HostButtonClicked);
	}

	if (JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &ThisClass::JoinButtonClicked);
	}

	return true;
}

void UMenu::NativeDestruct()
{
	MenuTearDown();
	Super::NativeDestruct();
}

void UMenu::HostButtonClicked()
{
	HostButton->SetIsEnabled(false);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.f,
			FColor::Yellow,
			FString(TEXT("Host Button Clicked"))
		);
	}

	if (MultiplayerSessionSubsystem)
	{
		MultiplayerSessionSubsystem->CreateSession(NumPublicConnections, MatchType);
	}
}

void UMenu::JoinButtonClicked()
{
	JoinButton->SetIsEnabled(false);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.f,
			FColor::Yellow,
			FString(TEXT("Join Button Clicked"))
		);
	}

	if (MultiplayerSessionSubsystem)
	{
		MultiplayerSessionSubsystem->FindSessions(10000);
	}
}

void UMenu::MenuTearDown()
{
	RemoveFromParent();
	UWorld* World = GetWorld();

	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			FInputModeGameOnly InputModeData;
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(false);
		}
	}

}

void UMenu::OnCreateSession(bool bWasSucessful)
{
	if (bWasSucessful)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			FString Url = FString::Printf(TEXT("%s?listen"), *LobbyPath);
			World->ServerTravel(Url);
		}
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.f,
			FColor::Yellow,
			FString::Printf(TEXT("OnCreateSession Called %d"), bWasSucessful)
		);
	}
	HostButton->SetIsEnabled(true);
}

void UMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSucessful)
{
	if (MultiplayerSessionSubsystem == nullptr)
	{
		return;
	}

	if (!bWasSucessful || SessionResults.Num() <= 1)
	{
		JoinButton->SetIsEnabled(true);
	}

	GEngine->AddOnScreenDebugMessage(
		-1,
		15.f,
		FColor::Yellow,
		FString::Printf(TEXT("OnFindSessions %d Num : %d"), bWasSucessful, SessionResults.Num())
	);

	for (const FOnlineSessionSearchResult& Result : SessionResults)
	{
		FString Id = Result.GetSessionIdStr();
		FString User = Result.Session.OwningUserName;
		FString SettingValue;
		Result.Session.SessionSettings.Get(FName("MatchType"), SettingValue);
		if (SettingValue == MatchType)
		{
			MultiplayerSessionSubsystem->JoinSession(Result);
		}
	}

	
}

void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	if (EOnJoinSessionCompleteResult::Type::UnknownError == Result)
	{
		JoinButton->SetIsEnabled(false);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Yellow,
				FString(TEXT("OnJoinSession Failed"))
			);
		}
		return;
	}

	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			FString Address;
			SessionInterface->GetResolvedConnectString(NAME_GameSession, Address);

			APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
			if (PlayerController)
			{
				if (GEngine)
				{
					GEngine->AddOnScreenDebugMessage(
						-1,
						15.f,
						FColor::Yellow,
						FString::Printf(TEXT("OnJoinSession Address: %s"), *Address)
					);
				}

				PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
			}
		}
	}
}

void UMenu::OnDestroySession(bool bWasSucessful)
{
}

void UMenu::OnStartSession(bool bWasSucessful)
{
}
