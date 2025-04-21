// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CCody.h"
#include "Guns/CGun.h"
#include "Guns/CSapGun.h"

ACCody::ACCody()
{
	ConstructorHelpers::FClassFinder<ACSapGun> tempGun(TEXT("/Script/Engine.Blueprint'/Game/KHH/Blueprints/BP_SapGun.BP_SapGun_C'"));
	if (tempGun.Succeeded()) GunBP = tempGun.Class;
	
}

void ACCody::BeginPlay()
{
	Super::BeginPlay();
	Gun = GetWorld()->SpawnActor<ACGun>(GunBP);
	if (Gun) Gun->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("GunSocket"));
}

void ACCody::DoFire()
{
	UE_LOG(LogTemp, Warning, TEXT("Fire"));
	if(Gun)
	{
		Gun->FireBullet(FireDest);
	}
}
