// Fill out your copyright notice in the Description page of Project Settings.


#include "Sap/CSapDrum.h"
#include "Guns/CSap.h"
#include "Guns/CMatchBullet.h"
#include "enemy/CGiantBeetleFSM.h"
#include "Components/CapsuleComponent.h"
#include "../../../../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraFunctionLibrary.h"
#include "../../../../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h"

// Sets default values
ACSapDrum::ACSapDrum()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("RootComp"));
	SetRootComponent(RootComp);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComp);

	RootComp->OnComponentBeginOverlap.AddDynamic(this, &ACSapDrum::OnSapDrumOverlap);

	ConstructorHelpers::FObjectFinder<UNiagaraSystem> TempSystem(TEXT("/Script/Niagara.NiagaraSystem'/Game/CatalystVFX/ParticleSystems/NE_Explosion_Human_System.NE_Explosion_Human_System'"));
	if (TempSystem.Succeeded()) ExplosionSys = TempSystem.Object;
}

// Called when the game starts or when spawned
void ACSapDrum::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACSapDrum::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACSapDrum::Explosion()
{
	FHitResult HitInfos;
	FVector CurPos = GetActorLocation();
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);

	bool bHit = GetWorld()->SweepSingleByProfile(HitInfos, CurPos, CurPos, FQuat::Identity, TEXT(""), FCollisionShape::MakeSphere(ExplosionRadius), params);

	ExplosionComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ExplosionSys, HitInfos.Location);

	if (bHit)
	{
		auto enemy = HitInfos.GetActor()->GetDefaultSubobjectByName(TEXT("fsm"));
		if (enemy)
		{
			auto enemyFSM = Cast<UCGiantBeetleFSM>(enemy);
			enemyFSM->OnDamageProcess(30);
		}
	}
}

void ACSapDrum::OnSapDrumOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto sap = Cast<ACSap>(OtherActor);
	if (sap)
	{
		SapGauge++;
		SapGauge = FMath::Clamp(SapGauge, 0 , MaxSapGage);
		sap->Destroy();
		return;
	}
	auto match = Cast<ACMatchBullet>(OtherActor);
	if (match)
	{
		if (SapGauge == MaxSapGage)
		{
			UE_LOG(LogTemp,Warning,TEXT("EXPLOSION!"));
			Explosion();
			//ExplosionComp->Deactivate();
			SapGauge = 0;
			match->Destroy();
			return;
		}
	}
}

