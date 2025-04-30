// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterSelectWidget.generated.h"

/**
 * 
 */
UCLASS()
class WAH_API UCharacterSelectWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	class UCHHGameInstance* gi;
	UPROPERTY()
	class ACHHPlayerController* controller;

	virtual void NativeConstruct() override;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UButton* btn_Cody;
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	class UButton* btn_May;	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* playerC;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* playerM;

	UFUNCTION()
	void OnClickedCody();
	
	UFUNCTION()
	void OnClickedMay();

	UFUNCTION()
	void updateCody(bool selected, FString playerName);
	UFUNCTION()
	void updateMay(bool selected, FString playerName);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bSelectCody = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bSelectMay = false;
};
