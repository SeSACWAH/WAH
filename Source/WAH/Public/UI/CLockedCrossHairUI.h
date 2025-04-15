// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CLockedCrossHairUI.generated.h"

UCLASS()
class WAH_API UCLockedCrossHairUI : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadWrite, Category = "UI", meta = (BindWidget))
	class UCanvasPanel* CanvasPanel;

	UPROPERTY(BlueprintReadWrite, Category = "UI", meta = (BindWidget))
	class UImage* CrosshairLocked;

	// Crosshair 위치 조정
	void  UpdateCrosshairPosition(FVector2D InPosition);
};
