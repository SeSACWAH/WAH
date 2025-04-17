// Fill out your copyright notice in the Description page of Project Settings.


#include "Guns/CMatchGun.h"
#include "Guns/CBullet.h"

ACMatchGun::ACMatchGun()
{
    ConstructorHelpers::FObjectFinder<USkeletalMesh> tmpMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Net/Match/MatchSniper.MatchSniper'"));
    if (tmpMesh.Succeeded()) GunMeshComp->SetSkeletalMesh(tmpMesh.Object);
}

void ACMatchGun::BeginPlay()
{
    Super::BeginPlay();
}

void ACMatchGun::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ACMatchGun::AddBulletToPool(bool bIsActivate)
{
	FActorSpawnParameters params;
	// 스폰 시 충돌이 생겨도 제자리에서 스폰되게 하는 기능
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	FTransform firePosition = GunMeshComp->GetSocketTransform(TEXT("FirePosition"));
	//UE_LOG(LogTemp, Warning, TEXT(">>>>> Socket Tranform : %s"), *GunMeshComp->GetSocketTransform(TEXT("FirePosition")).ToString());

	ACBullet* bullet = GetWorld()->SpawnActor<ACBullet>(BulletSpawner, firePosition, params);

	bullet->ActivateBullet(bIsActivate);
	bullet->SetCanMove(bIsActivate);
	BulletPool.Add(bullet);

	//UE_LOG(LogTemp, Warning, TEXT("[%d] : Add Bullet To Pool"), BulletPool.Num());
}
