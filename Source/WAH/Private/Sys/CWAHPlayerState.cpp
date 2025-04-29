// Fill out your copyright notice in the Description page of Project Settings.


#include "Sys/CWAHPlayerState.h"
#include "Sys/CHHGameInstance.h"

void ACWAHPlayerState::BeginPlay()
{
	Super::BeginPlay();

	if(GetPlayerController() && GetPlayerController()->IsLocalController())
	{
		auto gi = Cast<UCHHGameInstance>(GetWorld()->GetGameInstance());
		ServerRPC_SetUserName(gi->mySessionName);
	}
}

void ACWAHPlayerState::ServerRPC_SetUserName_Implementation(const FString& name)
{
	SetPlayerName(name);
}
