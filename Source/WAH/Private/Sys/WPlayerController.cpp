// Fill out your copyright notice in the Description page of Project Settings.


#include "Sys/WPlayerController.h"

void AWPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
	{
		bMay = true;
		ServerRPC_ChangePlayer(bMay);
	}
}

void AWPlayerController::ServerRPC_ChangePlayer_Implementation(bool IsMay)
{
	if (IsMay)
	{
		auto oldPawn = GetPawn();
		UnPossess();
		APawn* newChar = GetWorld()->SpawnActor<APawn>(PlayerFac, oldPawn->GetActorTransform());
		Possess(newChar);
		oldPawn->Destroy();
	}
}
