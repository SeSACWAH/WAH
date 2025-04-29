// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CChoosePlayerUI.generated.h"

/**
 * 
 */
UCLASS()
class WAH_API UCChoosePlayerUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UButton* btn_Cody;
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UButton* btn_May;

protected:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void OnButtonCodyClicked();
	UFUNCTION()
	void OnButtonMayClicked();
};
