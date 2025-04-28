// Fill out your copyright notice in the Description page of Project Settings.


#include "CWAHGameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "../../../../../../../Plugins/Online/OnlineBase/Source/Public/Online/OnlineSessionNames.h"


void UCWAHGameInstance::Init()
{
	if (auto subsys = IOnlineSubsystem::Get())
	{
		// ���� �ý������κ��� ���� �������̽� ��������
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
	// ���Ǽ��� ����
	FOnlineSessionSettings sessionSettings;

	// 1. Dedicated Server ���� ����
	sessionSettings.bIsDedicated = false;

	// 2. ����(����)��Ī�� ���� steam ��Ī�� ���� ����
	FName subsysName = IOnlineSubsystem::Get()->GetSubsystemName();
	sessionSettings.bIsLANMatch = (subsysName == "NULL");

	// 3. ��Ī�� �¶����� ���� ������� ����
	// false �̸� �ʴ븦 ���ؼ��� ������ ����
	// SendSessionInviteToFriend() �Լ��� ���� ģ���� �ʴ��� �� �ִ�.
	sessionSettings.bShouldAdvertise = true;

	// 4. �¶��� ����(Presence) ������ Ȱ������ ����
	sessionSettings.bUsesPresence = true;
	sessionSettings.bUseLobbiesIfAvailable = true;

	// 5. ���������߿� ���� �㰡���� ����
	sessionSettings.bAllowJoinViaPresence = true;
	sessionSettings.bAllowJoinInProgress = true;

	// 6. ���ǿ� ������ �� �ִ� ����(public) ������ �ִ� ��� ��
	sessionSettings.NumPublicConnections = playerCount;

	// 7. Ŀ���� ����� ����
	sessionSettings.Set(FName("ROOM_NAME"), StringBase64Encode(roomName), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	// 8. ȣ��Ʈ ���� ����
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

	// 1. ���� �˻� ���� ����
	sessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	// 2. Lan ����
	sessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == FName("NULL");

	// 3. �ִ� �˻� ���� ��
	sessionSearch->MaxSearchResults = 10;

	// 4. ���ǰ˻�
	sessionInterface->FindSessions(0, sessionSearch.ToSharedRef());

}

void UCWAHGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
	// ã�� ���н�
	if (!bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("Session search failed..."));
		return;
	}

	// ���ǰ˻���� �迭
	auto results = sessionSearch->SearchResults;
	UE_LOG(LogTemp, Warning, TEXT("Search Result Count : %d"), results.Num());

	// ������ �����´�
	for (int i = 0; i < results.Num(); ++i)
	{
		auto sr = results[i];

		// ������ ��ȿ���� üũ
		if (!sr.IsValid()) continue;

		FString roomName;
		FString hostName;

		FSessionInfo sessionInfo;
		sessionInfo.index = i;

		sr.Session.SessionSettings.Get(FName("ROOM_NAME"), roomName);
		sessionInfo.roomName = StringBase64Decode(roomName);

		sr.Session.SessionSettings.Get(FName("HOST_NAME"), hostName);
		sessionInfo.hostName = StringBase64Decode(hostName);

		// ���尡���� �÷��̾� ��
		int32 maxPlayerCount = sr.Session.SessionSettings.NumPublicConnections;
		// ���� ������ �÷��̾� �� ( �ִ� - ���� ���� ������ �� )
		// NumOpenPublicConnections ���������� ���������� ���� ���´�.
		int32 currentPlayerCount = maxPlayerCount - sr.Session.NumOpenPublicConnections;

		sessionInfo.playerCount = FString::Printf(TEXT("(%d/%d)"), currentPlayerCount, maxPlayerCount);

		// ������ (���������� 9999�� ���´�)
		int32 pingSpeed = sr.PingInMs;

		UE_LOG(LogTemp, Warning, TEXT("%s"), *sessionInfo.ToString());
	}

}

FString UCWAHGameInstance::StringBase64Encode(const FString& str)
{
	// Set �� �� : FStirng -> UTF8 (std::string) -> TArray<uint8> -> base64 �� Encode
	std::string utf8String = TCHAR_TO_UTF8(*str);
	TArray<uint8> arrayData = TArray<uint8>((uint8*)(utf8String.c_str()), utf8String.length());
	return FBase64::Encode(arrayData);
}

FString UCWAHGameInstance::StringBase64Decode(const FString& str)
{
	// Get�� �� (�������� �� �������� �޾ƿ� ��)
	// : base64�� Decode -> TArray<uint8> -> TCHAR
	TArray<uint8> arrayData;
	FBase64::Decode(str, arrayData);
	std::string utf8String((char*)(arrayData.GetData()), arrayData.Num());
	return UTF8_TO_TCHAR(utf8String.c_str());
}
