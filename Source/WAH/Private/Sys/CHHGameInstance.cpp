// Fill out your copyright notice in the Description page of Project Settings.


#include "Sys/CHHGameInstance.h"
#include "../../../../Plugins/Online/OnlineSubsystem/Source/Public/Interfaces/OnlineSessionInterface.h"
#include "../../../../Plugins/Online/OnlineSubsystem/Source/Public/OnlineSessionSettings.h"
#include "../../../../Plugins/Online/OnlineBase/Source/Public/Online/OnlineSessionNames.h"




void UCHHGameInstance::Init()
{
	if( auto subsys = IOnlineSubsystem::Get() )
	{
		SessionInterface = subsys->GetSessionInterface();

		SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this,&UCHHGameInstance::OnCreateSessionComplete);
		SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this,&UCHHGameInstance::OnFindSessionsComplete);
	}
}

void UCHHGameInstance::CreateMySession(FString roomName)
{
	FOnlineSessionSettings sessionSettings;

	sessionSettings.bIsDedicated = false;

	FName subsysname = IOnlineSubsystem::Get()->GetSubsystemName();
	sessionSettings.bIsLANMatch = (subsysname == "NULL");

	// SendSessionInviteToFriend()
	sessionSettings.bShouldAdvertise = true;
	
	sessionSettings.bUsesPresence = true;
	sessionSettings.bUseLobbiesIfAvailable = true;

	sessionSettings.bAllowJoinViaPresence = true;
	sessionSettings.bAllowJoinInProgress = true;

	sessionSettings.NumPublicConnections = 2;

	sessionSettings.Set(FName("ROOM_NAME"), roomName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	sessionSettings.Set(FName("HOST_NAME"), mySessionName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	
	FUniqueNetIdPtr netID = GetWorld()->GetFirstLocalPlayerFromController()->GetUniqueNetIdForPlatformUser().GetUniqueNetId();

	SessionInterface->CreateSession(*netID, FName(mySessionName),sessionSettings);
}

void UCHHGameInstance::OnCreateSessionComplete(FName sessionName, bool bWasSuccessful)
{

}

void UCHHGameInstance::FindOtherSession()
{
	sessionSearch = MakeShareable(new FOnlineSessionSearch());

	sessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	sessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == FName("NULL");

	sessionSearch->MaxSearchResults = 10;

	SessionInterface->FindSessions(0, sessionSearch.ToSharedRef());
}

void UCHHGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
	if(!bWasSuccessful) return;

	auto results = sessionSearch->SearchResults;

	for(int i=0; i<results.Num(); ++i)
	{
		auto sr = results[i];

		if(!sr.IsValid()) continue;

		FHSessionInfo sessionInfo;
		sessionInfo.index = i;

		sr.Session.SessionSettings.Get(FName("ROOM_NAME"), sessionInfo.roomName);
		
		sr.Session.SessionSettings.Get(FName("HOST_NAME"), sessionInfo.hostName);
		
	}
}
