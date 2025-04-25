// Fill out your copyright notice in the Description page of Project Settings.


#include "Sys/WPlayerController.h"
#include "../../../../../../../Source/Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "CWAHGameMode.h"
#include "../../../../../../../Source/Runtime/Engine/Public/Net/UnrealNetwork.h"

void AWPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void AWPlayerController::SetChooseUIRef(UUserWidget* InWidget)
{
	ChooseUI = InWidget;
}

void AWPlayerController::ServerRPC_RequestSpawn_Implementation(EPlayerRole InPlayerRole)
{
	SelectedRole = InPlayerRole;

	if (ACWAHGameMode* gm = GetWorld()->GetAuthGameMode<ACWAHGameMode>())
		gm->SpawnPlayerCharacter(this);

	ClientRPC_RemoveChoosePlayerUI();
}

void AWPlayerController::ClientRPC_RemoveChoosePlayerUI_Implementation()
{
	if (ChooseUI)
	{
		ChooseUI->RemoveFromParent();
		ChooseUI->SetVisibility(ESlateVisibility::Hidden);
		bShowMouseCursor = false;
		this->SetInputMode(FInputModeGameOnly());
	}
}


void AWPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWPlayerController, SelectedRole);
}