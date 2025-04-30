// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SessionSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class WAH_API USessionSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
	public:
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* txt_roomName;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* txt_hostName;

	int32 SessionNumber;

	void Set(const struct FHSessionInfo& InSessionInfo );

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	class UButton* btn_join;

	virtual void NativeConstruct() override;

	UFUNCTION()
	void JoinSession();
};
