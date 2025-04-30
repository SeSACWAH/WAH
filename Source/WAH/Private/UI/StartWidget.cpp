// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/StartWidget.h"
#include "Components/EditableText.h"
#include "Sys/CHHGameInstance.h"
#include "Components/WidgetSwitcher.h"
#include "Components/Button.h"
#include "UI/CButtonWidget.h"
#include "UI/SessionSlotWidget.h"
#include "Components/ScrollBox.h"
#include "UI/CChoosePlayerUI.h"



void UStartWidget::NativeConstruct()
{
	Super::NativeConstruct();

	gi = Cast<UCHHGameInstance>(GetWorld()->GetGameInstance());
	gi->onSearchCompleted.AddDynamic(this, &UStartWidget::AddSlotWidget);

	btn_gameStart->OnClickedDelegate.BindUFunction(this, FName("SwitchCreatePanel"));
	btn_findRoom->OnClickedDelegate.BindUFunction(this, FName("SwitchFindPanel"));
	btn_back->OnClickedDelegate.BindUFunction(this, FName("BackToMain"));
	btn_back2->OnClickedDelegate.BindUFunction(this, FName("BackToMain"));
	btn_createRoom->OnClickedDelegate.BindUFunction(this, FName("CreateRoom"));
	btn_Cody->OnClicked.AddDynamic(this, &UStartWidget::OnClickedCody);
	btn_May->OnClicked.AddDynamic(this, &UStartWidget::OnClickedMay);
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
	OnClickedFindSession();
}

void UStartWidget::BackToMain()
{
	ws->SetActiveWidgetIndex(0);
}

void UStartWidget::OnClickedFindSession()
{
	scroll_roomList->ClearChildren();
	if(gi!=nullptr)
	{
		gi->FindOtherSession();
	}
}

void UStartWidget::OnClickedCody()
{
	gi->bTypeCody = true;
}

void UStartWidget::OnClickedMay()
{
	gi->bTypeCody = false;
}

void UStartWidget::AddSlotWidget(const struct FHSessionInfo& InSessionInfo)
{
	auto slot = CreateWidget<USessionSlotWidget>(this, SeessionInfoWidget);
	slot->Set(InSessionInfo);

	scroll_roomList->AddChild(slot);
}
