// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SessionSlotWidget.h"
#include "CWAHGameInstance.h"
#include "Components/TextBlock.h"
#include "Sys/CHHGameInstance.h"
#include "Components/Button.h"

void USessionSlotWidget::Set(const struct FHSessionInfo& InSessionInfo)
{
	txt_roomName->SetText(FText::FromString(InSessionInfo.roomName));
	txt_hostName->SetText(FText::FromString(InSessionInfo.hostName));
	SessionNumber = InSessionInfo.index;
}

void USessionSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	btn_join->OnClicked.AddDynamic(this, &USessionSlotWidget::JoinSession);

}

void USessionSlotWidget::JoinSession()
{
	auto gi = Cast<UCHHGameInstance>(GetWorld()->GetGameInstance());
	if (gi)
	{
		gi->JoinSelectedSession(SessionNumber);
	}

}
