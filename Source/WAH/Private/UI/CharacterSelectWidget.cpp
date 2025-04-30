// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CharacterSelectWidget.h"
#include "Sys/CHHGameInstance.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Sys/CHHPlayerController.h"

void UCharacterSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	gi = Cast<UCHHGameInstance>(GetWorld()->GetGameInstance());
	

	btn_Cody->OnClicked.AddDynamic(this, &UCharacterSelectWidget::OnClickedCody);
	btn_May->OnClicked.AddDynamic(this, &UCharacterSelectWidget::OnClickedMay);
}

void UCharacterSelectWidget::OnClickedCody()
{
	//updateCody(bSelectCody, gi->mySessionName);
	//controller->ServerRPC_SelectCody(bSelectCody);
}

void UCharacterSelectWidget::OnClickedMay()
{	
	/*if (!bSelectMay)
	{
		gi->bTypeCody = false;
		bSelectMay = true;
		btn_May->SetBackgroundColor(FColor::Yellow);
		playerM->SetText(FText::FromString(gi->mySessionName));
	}
	else
	{
		bSelectMay = false;
		btn_May->SetBackgroundColor(FColor::Emerald);
		playerM->SetText(FText::GetEmpty());
	}*/
	//updateMay(bSelectMay, gi->mySessionName);
	UE_LOG(LogTemp, Warning, TEXT("Click"));
	//controller->ServerRPC_SelectMay(bSelectMay);
}

void UCharacterSelectWidget::updateCody(bool selected, FString playerName)
{
	if (!selected)
	{
		gi->bTypeCody = true;
		bSelectCody = true;
		btn_Cody->SetBackgroundColor(FColor::Yellow);
		playerC->SetText(FText::FromString(playerName));
	}
	else
	{
		bSelectCody = false;
		btn_Cody->SetBackgroundColor(FColor::Emerald);
		playerC->SetText(FText::GetEmpty());
	}
}

void UCharacterSelectWidget::updateMay(bool selected, FString playerName)
{
	if (selected)
	{
		gi->bTypeCody = false;
		//bSelectMay = false;
		btn_May->SetBackgroundColor(FColor::Yellow);
		playerM->SetText(FText::FromString(playerName));
	}
	else
	{
		//bSelectMay = true;
		btn_May->SetBackgroundColor(FColor::Emerald);
		playerM->SetText(FText::GetEmpty());
	}
}
