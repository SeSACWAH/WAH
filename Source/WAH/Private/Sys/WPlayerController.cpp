// Fill out your copyright notice in the Description page of Project Settings.


#include "Sys/WPlayerController.h"
#include "Player/CCody.h"
#include "Guns/CGun.h"
#include "Kismet/GameplayStatics.h"
#include "../../../../../../../Source/Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "CWAHGameMode.h"
#include "../../../../../../../Source/Runtime/Engine/Public/Net/UnrealNetwork.h"

void AWPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

	if (!HasAuthority())
	{
		bMay = true;
		ServerRPC_ChangePlayer(bMay);
	}
void AWPlayerController::SetChooseUIRef(UUserWidget* InWidget)
{
	ChooseUI = InWidget;
}

void AWPlayerController::ServerRPC_ChangePlayer_Implementation(bool IsMay)
void AWPlayerController::ServerRPC_RequestSpawn_Implementation(EPlayerRole InPlayerRole)
{
	SelectedRole = InPlayerRole;

	if(ACWAHGameMode* gm = GetWorld()->GetAuthGameMode<ACWAHGameMode>())
		gm->SpawnPlayerCharacter(this);

		ClientRPC_RemoveChoosePlayerUI();
}

void AWPlayerController::ClientRPC_RemoveChoosePlayerUI_Implementation()
{
	if (ChooseUI)
	{
		auto oldPawn = GetPawn();
		UnPossess();
		APawn* newChar = GetWorld()->SpawnActor<APawn>(PlayerFac, oldPawn->GetActorTransform());
		Possess(newChar);
		ACCody* cody = Cast<ACCody>(oldPawn);
		cody->Gun->SetActorHiddenInGame(true);
		oldPawn->Destroy();
		TArray<AActor*> OutActors;
		UGameplayStatics::GetAllActorsWithTag(GetWorld(),TEXT("SSCamera"), OutActors );
		AActor* cam = OutActors[FMath::RandRange(0, OutActors.Num() - 1)];
		SetViewTarget(cam);
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