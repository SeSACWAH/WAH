// Fill out your copyright notice in the Description page of Project Settings.


#include "Guns/CSap.h"
#include "Player/CPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Player/CCody.h"

void ACSap::BeginPlay()
{
	auto cody = Cast<ACCody>(UGameplayStatics::GetPlayerPawn(GetWorld(),0));

	FireDestination = cody->FireDest;

	bCanMove = true;
}

void ACSap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACSap::DoMoveBullet(float InDeltaTime)
{
	CurFireTime += InDeltaTime;


}
