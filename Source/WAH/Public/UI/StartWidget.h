// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StartWidget.generated.h"

/**
 * 
 */
UCLASS()
class WAH_API UStartWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UCButtonWidget* btn_gameStart;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UCButtonWidget* btn_findRoom;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UCButtonWidget* btn_createRoom;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UEditableText* edit_roomName;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UWidgetSwitcher* ws;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UCButtonWidget* btn_findSession;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	//class UButton* btn_findSession;

	UPROPERTY()
	class UCHHGameInstance* gi;

	virtual void NativeConstruct() override;

	UFUNCTION()
	void CreateRoom();


	UFUNCTION()
	void SwitchCreatePanel();
	UFUNCTION()
	void SwitchFindPanel();
	UFUNCTION()
	void BackToMain();
};
