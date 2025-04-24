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
#include "../../../../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraFunctionLibrary.h"
#include "../../../../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h"

ACSap::ACSap()
{
	MoveComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MoveComp"));
	BulletComp->SetCollisionProfileName(TEXT("Sap"));
	BulletComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MoveComp->SetUpdatedComponent(RootComponent);

	BulletComp->OnComponentBeginOverlap.AddDynamic(this, &ACSap::OnSapOverlap);

	ConstructorHelpers::FObjectFinder<UNiagaraSystem> TempSystem(TEXT("/Script/Niagara.NiagaraSystem'/Game/CatalystVFX/ParticleSystems/NE_Explosion_Human_System.NE_Explosion_Human_System'"));
	if(TempSystem.Succeeded()) ExplosionSys = TempSystem.Object;
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

void ACSap::Explosion(FVector lot)
{
	ServerRPC_Explosion(lot);
}

void ACSap::ServerRPC_Explosion_Implementation(FVector lot)
{
	MultiRPC_Explosion(lot);
}

void ACSap::MultiRPC_Explosion_Implementation(FVector lot)
{
	ExplosionComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ExplosionSys, GetActorLocation());
}

void ACSap::OnSapOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!HasAuthority()) return;
	// beetle
	auto beetle = Cast<ACGiantBeetle>(OtherActor);
	if(beetle)
	{
		Destroy();
		return;
	}
	// sap 
	auto sap = Cast<ACSap>(OtherActor);
	if(sap)
	{
		sap->SapGather++;
		sap->SapGather = FMath::Clamp(sap->SapGather, 1, 3) / 2;
		sap->SetSapGather(sap->SapGather);
		if(SapGather == 0) Destroy();
		return;
	}

	// ¼º³É
	ACMatchBullet* matchBullet = Cast<ACMatchBullet>(OtherActor);
	if(matchBullet)
	{
		Explosion(SweepResult.Location);
		//ExplosionComp->Deactivate();
		Destroy();
		matchBullet->ActivateBullet(false);
		return;
	}
	
	// ¹Ù´Ú
	auto floor = Cast<AActor>(OtherActor);
	if (floor)
	{
		MoveComp->StopMovementImmediately();
		SetActorLocation(SweepResult.Location);
		SapGather = 1;
	}

}

