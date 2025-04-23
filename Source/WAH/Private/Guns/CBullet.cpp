#include "Guns/CBullet.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"

ACBullet::ACBullet()
{
	PrimaryActorTick.bCanEverTick = true;

	/* Network */
	bReplicates = true;
	SetReplicateMovement(true);

	/* Collision*/
	BulletComp = CreateDefaultSubobject<USphereComponent>(TEXT("BulletComp"));
	SetRootComponent(BulletComp);
	BulletComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	/* Mesh */
	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletMesh"));
	BulletMesh->SetupAttachment(RootComponent);
}

void ACBullet::BeginPlay()
{
	Super::BeginPlay();
}

void ACBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		if(bCanMove) DoMoveBullet(DeltaTime);
	}
}

USphereComponent* ACBullet::GetBulletComp() const
{
	return nullptr;
}

UStaticMeshComponent* ACBullet::GetBulletMesh() const
{
	return nullptr;
}

void ACBullet::SetFireDestination(FVector InDirection)
{
}

void ACBullet::SetCanMove(bool InResult)
{
}

void ACBullet::ActivateBullet(bool bIsActivate)
{
}

void ACBullet::DoMoveBullet(float InDeltaTime)
{
}

void ACBullet::CompleteMoveBullet(FVector InDestination)
{
}

void ACBullet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(ACBullet, );
	DOREPLIFETIME(ACBullet, bCanMove);
	DOREPLIFETIME(ACBullet, FireDestination);
}