// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Delegates/DelegateCombinations.h"
#include "CButtonWidget.generated.h"

DECLARE_DELEGATE(FBtnClicked);

/**
 * 
 */
UCLASS()
class WAH_API UCButtonWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UButton* btn;

	FBtnClicked OnClickedDelegate;

	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnClicked();
};
