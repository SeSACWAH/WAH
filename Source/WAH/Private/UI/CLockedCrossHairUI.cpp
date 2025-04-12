// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CLockedCrossHairUI.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"

void UCLockedCrossHairUI::UpdateCrosshairPosition(FVector2D InPosition)
{
    UCanvasPanelSlot* canvasSlot = Cast<UCanvasPanelSlot>(CrosshairLocked->Slot);
    
    if(canvasSlot) canvasSlot->SetPosition(InPosition);
}
