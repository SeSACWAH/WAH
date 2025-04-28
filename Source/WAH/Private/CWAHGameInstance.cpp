// Fill out your copyright notice in the Description page of Project Settings.


#include "CWAHGameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "../../../../../../../Plugins/Online/OnlineBase/Source/Public/Online/OnlineSessionNames.h"


void UCWAHGameInstance::Init()
{
	if (auto subsys = IOnlineSubsystem::Get())
	{
		// 서브 시스템으로부터 세션 인터페이스 가져오기
		sessionInterface = subsys->GetSessionInterface();

		sessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UCWAHGameInstance::OnCreateSessionComplete);
		sessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this,
			&UCWAHGameInstance::OnFindSessionsComplete);

		FTimerHandle handle;
		GetWorld()->GetTimerManager().SetTimer(handle, FTimerDelegate::CreateLambda([&]
			{
				CreateMySession(mySessionName, 10);

			}
		), 2.0f, false);

		FTimerHandle handle2;
		GetWorld()->GetTimerManager().SetTimer(handle2, FTimerDelegate::CreateLambda([&]
			{
				FindOtherSession();
			}), 2.0f, false);

	}
}

void UCWAHGameInstance::CreateMySession(FString roomName, int32 playerCount)
{
	// 세션설정 변수
	FOnlineSessionSettings sessionSettings;

	// 1. Dedicated Server 접속 여부
	sessionSettings.bIsDedicated = false;

	// 2. 랜선(로컬)매칭을 할지 steam 매칭을 할지 여부
	FName subsysName = IOnlineSubsystem::Get()->GetSubsystemName();
	sessionSettings.bIsLANMatch = (subsysName == "NULL");

	// 3. 매칭이 온라인을 통해 노출될지 여부
	// false 이면 초대를 통해서만 입장이 가능
	// SendSessionInviteToFriend() 함수를 통해 친구를 초대할 수 있다.
	sessionSettings.bShouldAdvertise = true;

	// 4. 온라인 상태(Presence) 정보를 활용할지 여부
	sessionSettings.bUsesPresence = true;
	sessionSettings.bUseLobbiesIfAvailable = true;

	// 5. 게임진행중에 참여 허가할지 여부
	sessionSettings.bAllowJoinViaPresence = true;
	sessionSettings.bAllowJoinInProgress = true;

	// 6. 세션에 참여할 수 있는 공개(public) 연결의 최대 허용 수
	sessionSettings.NumPublicConnections = playerCount;

	// 7. 커스텀 룸네임 설정
	sessionSettings.Set(FName("ROOM_NAME"), StringBase64Encode(roomName), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	// 8. 호스트 네임 설정
	sessionSettings.Set(FName("HOST_NAME"), StringBase64Encode(mySessionName), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	// netID
	FUniqueNetIdPtr netID = GetWorld()->GetFirstLocalPlayerFromController()->GetUniqueNetIdForPlatformUser().GetUniqueNetId();

	UE_LOG(LogTemp, Warning, TEXT("Create Session Strat : %s"), *mySessionName);
	sessionInterface->CreateSession(*netID, FName(mySessionName), sessionSettings);

}

void UCWAHGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("Session Name : %s, bWasSuccessful : %d"), *SessionName.ToString(), bWasSuccessful);
}

void UCWAHGameInstance::FindOtherSession()
{
	sessionSearch = MakeShareable(new FOnlineSessionSearch());

	// 1. 세션 검색 조건 설정
	sessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	// 2. Lan 여부
	sessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == FName("NULL");

	// 3. 최대 검색 세션 수
	sessionSearch->MaxSearchResults = 10;

	// 4. 세션검색
	sessionInterface->FindSessions(0, sessionSearch.ToSharedRef());

}

void UCWAHGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
	// 찾기 실패시
	if (!bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("Session search failed..."));
		return;
	}

	// 세션검색결과 배열
	auto results = sessionSearch->SearchResults;
	UE_LOG(LogTemp, Warning, TEXT("Search Result Count : %d"), results.Num());

	// 정보를 가져온다
	for (int i = 0; i < results.Num(); ++i)
	{
		auto sr = results[i];

		// 정보가 유효하지 체크
		if (!sr.IsValid()) continue;

		FString roomName;
		FString hostName;

		FSessionInfo sessionInfo;
		sessionInfo.index = i;

		sr.Session.SessionSettings.Get(FName("ROOM_NAME"), roomName);
		sessionInfo.roomName = StringBase64Decode(roomName);

		sr.Session.SessionSettings.Get(FName("HOST_NAME"), hostName);
		sessionInfo.hostName = StringBase64Decode(hostName);

		// 입장가능한 플레이어 수
		int32 maxPlayerCount = sr.Session.SessionSettings.NumPublicConnections;
		// 현재 입장한 플레이어 수 ( 최대 - 현재 입장 가능한 수 )
		// NumOpenPublicConnections 스팀에서만 정상적으로 값이 들어온다.
		int32 currentPlayerCount = maxPlayerCount - sr.Session.NumOpenPublicConnections;

		sessionInfo.playerCount = FString::Printf(TEXT("(%d/%d)"), currentPlayerCount, maxPlayerCount);

		// 핑정보 (스팀에서는 9999로 나온다)
		int32 pingSpeed = sr.PingInMs;

		UE_LOG(LogTemp, Warning, TEXT("%s"), *sessionInfo.ToString());
	}

}

FString UCWAHGameInstance::StringBase64Encode(const FString& str)
{
	// Set 할 때 : FStirng -> UTF8 (std::string) -> TArray<uint8> -> base64 로 Encode
	std::string utf8String = TCHAR_TO_UTF8(*str);
	TArray<uint8> arrayData = TArray<uint8>((uint8*)(utf8String.c_str()), utf8String.length());
	return FBase64::Encode(arrayData);
}

FString UCWAHGameInstance::StringBase64Decode(const FString& str)
{
	// Get할 때 (서버에서 방 정보들을 받아올 때)
	// : base64로 Decode -> TArray<uint8> -> TCHAR
	TArray<uint8> arrayData;
	FBase64::Decode(str, arrayData);
	std::string utf8String((char*)(arrayData.GetData()), arrayData.Num());
	return UTF8_TO_TCHAR(utf8String.c_str());
}
