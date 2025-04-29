// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CharacterSelectWidget.h"
#include "Sys/CHHGameInstance.h"
#include "Components/Button.h"

void UCharacterSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	btn_Cody->OnClicked.AddDynamic(this, &UCharacterSelectWidget::OnClickedCody);
}

void UCharacterSelectWidget::OnClickedCody()
{
	if(!bSelectCody)
	{
		gi->bTypeCody = true;
		if(bSelectMay) GetWorld()->ServerTravel(TEXT("/Game/Common/Maps/BattleMap?listen"));
		bSelectCody = true;
	}
	else
	{
		bSelectCody = false;
	}
}

void UCharacterSelectWidget::OnClickedMay()
{
	if(!bSelectMay)
	{
		gi->bTypeCody = false;
		if (bSelectMay) GetWorld()->ServerTravel(TEXT("/Game/Common/Maps/BattleMap?listen"));
		bSelectMay = true;
	}
	else
	{
		bSelectMay = false;
	}
}
