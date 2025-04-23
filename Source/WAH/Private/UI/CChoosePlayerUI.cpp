// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CChoosePlayerUI.h"
#include "../../../../../../../Source/Runtime/UMG/Public/Components/Button.h"
#include "Player/CCody.h"
#include "Player/CMay.h"

void UCChoosePlayerUI::NativeConstruct()
{
    Super::NativeConstruct();

    Btn_Cody->OnClicked.AddDynamic(this, &UCChoosePlayerUI::OnButtonCodyClicked);
    Btn_May->OnClicked.AddDynamic(this, &UCChoosePlayerUI::OnButtonMayClicked);
}

void UCChoosePlayerUI::OnButtonCodyClicked()
{
    //if(auto pc = GetOwningPlayer<ACCody>())
    //    pc->ServerRequestSpawn(EplayerRole::Cody);
}

void UCChoosePlayerUI::OnButtonMayClicked()
{
    //if (auto pc = GetOwningPlayer<ACMay>())
    //    pc->ServerRequestSpawn(EplayerRole::May);
}
