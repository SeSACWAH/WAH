// Fill out your copyright notice in the Description page of Project Settings.


#include "Guns/CSapGun.h"
#include "Guns/CSap.h"

ACSapGun::ACSapGun()
{
	GunMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>( TEXT("GunMeshComp") );
	SetRootComponent(GunMeshComp);

	ConstructorHelpers::FObjectFinder<USkeletalMesh> tmpMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Net/SapGun/SapGun.SapGun'"));
	if (tmpMesh.Succeeded()) GunMeshComp->SetSkeletalMesh(tmpMesh.Object);
}

void ACSapGun::Fire()
{
	FTransform firePosition = GunMeshComp->GetSocketTransform(TEXT("FirePosition"));

	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	GetWorld()->SpawnActor<ACSap>(SapFac, firePosition, params);
	
}

void ACSapGun::FireBullet(FVector InDestination)
{
	Fire();
}
