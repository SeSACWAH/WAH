// Fill out your copyright notice in the Description page of Project Settings.

#include "Sys/WAHGameViewportClient.h"
#include "Engine/ViewportSplitScreen.h"

UWAHGameViewportClient::UWAHGameViewportClient() : Super(FObjectInitializer::Get())
{
	SplitscreenInfo.SetNum(ECustomSplitScreenType::SplittypeCount, false);
	
	SplitscreenInfo[ESplitScreenType::None].PlayerData.Add(FPerPlayerSplitscreenData(0.5f, 1.0f, 0.0f, 0.0f));
	SplitscreenInfo[ESplitScreenType::None].PlayerData.Add(FPerPlayerSplitscreenData(0.5f, 1.0f, 0.5f, 0.0f));
}

void UWAHGameViewportClient::UpdateActiveSplitscreenType()
{
	ECustomSplitScreenType::Type SplitType = ECustomSplitScreenType:: None;
	const int32  NumPlayers = GEngine->GetNumGamePlayers(GetWorld())+1;

	if (NumPlayers < 3)
	{
		ActiveSplitscreenType = static_cast<ESplitScreenType::Type>(ECustomSplitScreenType::None);
	}
	else
	{
		Super::UpdateActiveSplitscreenType();
	}
}
