// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameViewportClient.h"
#include "WAHGameViewportClient.generated.h"

UENUM(BlueprintType)
namespace ECustomSplitScreenType
{
	enum Type
	{
		None,
		TwoPlayer = ESplitScreenType::SplitTypeCount,
		SplittypeCount
	};
}

/**
 * 
 */
UCLASS()
class WAH_API UWAHGameViewportClient : public UGameViewportClient
{
	GENERATED_BODY()
	
public:
	UWAHGameViewportClient();


	virtual void UpdateActiveSplitscreenType() override;
};
