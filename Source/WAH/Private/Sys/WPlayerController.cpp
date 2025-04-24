// Fill out your copyright notice in the Description page of Project Settings.


#include "Sys/WPlayerController.h"
#include "Player/CCody.h"
#include "Guns/CGun.h"
#include "Kismet/GameplayStatics.h"

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
		ACCody* cody = Cast<ACCody>(oldPawn);
		cody->Gun->SetActorHiddenInGame(true);
		oldPawn->Destroy();
		TArray<AActor*> OutActors;
		UGameplayStatics::GetAllActorsWithTag(GetWorld(),TEXT("SSCamera"), OutActors );
		AActor* cam = OutActors[FMath::RandRange(0, OutActors.Num() - 1)];
		SetViewTarget(cam);
	}
}
