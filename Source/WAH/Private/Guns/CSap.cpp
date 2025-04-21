// Fill out your copyright notice in the Description page of Project Settings.


#include "Guns/CSap.h"
#include "Player/CPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Player/CCody.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Guns/CMatchBullet.h"
#include "enemy/CGiantBeetle.h"
#include "enemy/CGiantBeetleFSM.h"

ACSap::ACSap()
{
	MoveComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MoveComp"));
	BulletComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MoveComp->SetUpdatedComponent(RootComponent);

	BulletComp->OnComponentBeginOverlap.AddDynamic(this, &ACSap::OnSapOverlap);
}

void ACSap::BeginPlay()
{
	Super::BeginPlay();
	//auto cody = Cast<ACCody>(UGameplayStatics::GetPlayerPawn(GetWorld(),0));

	//FireDestination = cody->FireDest;

	bCanMove = true;
}

void ACSap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACSap::SetSapGather(float sapGather)
{
	SetActorScale3D(FVector(sapGather));
}

void ACSap::Explosion()
{
	FHitResult HitInfos;
	FVector CurPos = GetActorLocation();
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);
	
	bool bHit = GetWorld()->SweepSingleByProfile(HitInfos, CurPos, CurPos, FQuat::Identity, TEXT(""), FCollisionShape::MakeSphere(ExplosionRadius), params);
	
	if(bHit)
	{
		auto enemy = HitInfos.GetActor()->GetDefaultSubobjectByName(TEXT("fsm"));
		if(enemy)
		{
			auto enemyFSM = Cast<UCGiantBeetleFSM>(enemy);
			enemyFSM->OnDamageProcess(30);
		}
	}
}

void ACSap::OnSapOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// sap 
	auto sap = Cast<ACSap>(OtherActor);
	if(sap)
	{
		sap->SapGather++;
		sap->SapGather = FMath::Clamp(sap->SapGather, 1, 3);
		sap->SetSapGather(sap->SapGather);
		Destroy();
	}

	// ¹Ù´Ú
	//auto floor = Cast

	// ¼º³É
	ACMatchBullet* matchBullet = Cast<ACMatchBullet>(OtherActor);
	if(matchBullet)
	{


	}

}

