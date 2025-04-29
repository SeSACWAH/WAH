// Fill out your copyright notice in the Description page of Project Settings.


#include "Sys/CHHPlayerController.h"
#include "Sys/CHHGameInstance.h"

void ACHHPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if(IsLocalController())
	{
		auto gi = Cast<UCHHGameInstance>(GetWorld()->GetGameInstance());
		ServerRPC_ChangePlayer(gi->bTypeCody);
	}
}

void ACHHPlayerController::ServerRPC_ChangePlayer_Implementation(bool bTypeCody)
{
	if(bTypeCody)
	{
		auto oldPawn = GetPawn();
		UnPossess();
		APawn* newPawn = GetWorld()->SpawnActor<APawn>(CodyFactory, oldPawn->GetActorTransform());
		Possess(newPawn);
		oldPawn->Destroy();

	}
}
