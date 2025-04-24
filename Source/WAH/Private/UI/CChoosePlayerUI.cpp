// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CChoosePlayerUI.h"
#include "../../../../../../../Source/Runtime/UMG/Public/Components/Button.h"
#include "Player/CCody.h"
#include "Player/CMay.h"
#include "Sys/WPlayerController.h"

void UCChoosePlayerUI::NativeConstruct()
{
    Super::NativeConstruct();

    Btn_Cody->OnClicked.AddDynamic(this, &UCChoosePlayerUI::OnButtonCodyClicked);
    Btn_May->OnClicked.AddDynamic(this, &UCChoosePlayerUI::OnButtonMayClicked);
}

void UCChoosePlayerUI::OnButtonCodyClicked()
{
    if(auto pc = GetOwningPlayer<AWPlayerController>())
        pc->ServerRPC_RequestSpawn(EPlayerRole::Cody);
}

void UCChoosePlayerUI::OnButtonMayClicked()
{
    if (auto pc = GetOwningPlayer<AWPlayerController>())
        pc->ServerRPC_RequestSpawn(EPlayerRole::May);
}
