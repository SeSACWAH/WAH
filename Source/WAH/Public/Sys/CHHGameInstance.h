// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "../../../../Plugins/Online/OnlineSubsystem/Source/Public/Interfaces/OnlineSessionInterface.h"
#include "CHHGameInstance.generated.h"

USTRUCT(BlueprintType)
struct FHSessionInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString roomName;
	UPROPERTY(BlueprintReadOnly)
	FString hostName;
	UPROPERTY(BlueprintReadOnly)
	int32 index;

};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSearchSignature, const FHSessionInfo&, sessionInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSearchStateSignature, bool, bIsSearching);

/**
 * 
 */
UCLASS()
class WAH_API UCHHGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	virtual void Init() override;

	IOnlineSessionPtr SessionInterface;

	void CreateMySession(FString roomName);

	FString mySessionName = "KH2";

	UFUNCTION()
	void OnCreateSessionComplete(FName sessionName, bool bWasSuccessful);


	TSharedPtr<FOnlineSessionSearch> sessionSearch;

	void FindOtherSession();

	void OnFindSessionsComplete(bool bWasSuccessful);

	FSearchSignature onSearchCompleted;

	FSearchStateSignature onSearchState;

	void JoinSelectedSession(int32 index);

	void OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result);

	bool bTypeCody;
};
