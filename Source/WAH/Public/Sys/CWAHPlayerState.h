// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "CWAHPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class WAH_API ACWAHPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	void ServerRPC_SetUserName(const FString& name);
};
