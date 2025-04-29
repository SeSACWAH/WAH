// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CButtonWidget.h"
#include "Components/Button.h"

void UCButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();

	btn->OnClicked.AddDynamic(this, &UCButtonWidget::OnClicked);
}

void UCButtonWidget::OnClicked()
{
	UE_LOG(LogTemp,Warning,TEXT("Click!"));
	if(OnClickedDelegate.IsBound())
		UE_LOG(LogTemp,Warning,TEXT("Bind!"));
	OnClickedDelegate.ExecuteIfBound();
}
