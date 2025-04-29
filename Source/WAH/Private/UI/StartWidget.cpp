// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/StartWidget.h"
#include "Components/EditableText.h"
#include "Sys/CHHGameInstance.h"
#include "Components/WidgetSwitcher.h"
#include "Components/Button.h"
#include "UI/CButtonWidget.h"

void UStartWidget::NativeConstruct()
{
	Super::NativeConstruct();

	btn_gameStart->OnClickedDelegate.BindUFunction(this, FName("SwitchCreatePanel"));
	btn_findRoom->OnClickedDelegate.BindUFunction(this, FName("SwitchFindPanel"));
	btn_back->OnClickedDelegate.BindUFunction(this, FName("BackToMain"));
	btn_back2->OnClickedDelegate.BindUFunction(this, FName("BackToMain"));

}

void UStartWidget::CreateRoom()
{
	if(!gi || edit_roomName->GetText().IsEmpty()) return;
	
	gi->CreateMySession(edit_roomName->GetText().ToString());
}

void UStartWidget::SwitchCreatePanel()
{
	ws->SetActiveWidgetIndex(1);
}

void UStartWidget::SwitchFindPanel()
{
	ws->SetActiveWidgetIndex(2);
}

void UStartWidget::BackToMain()
{
	ws->SetActiveWidgetIndex(0);
}
