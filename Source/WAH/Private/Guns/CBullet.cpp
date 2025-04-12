#include "Guns/CBullet.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Components/SphereComponent.h"

ACBullet::ACBullet()
{
	PrimaryActorTick.bCanEverTick = true;

	/* Collision*/
	BulletComp = CreateDefaultSubobject<USphereComponent>(TEXT("BulletComp"));
	SetRootComponent(BulletComp);
	//BulletComp->SetCollisionProfileName(TEXT("Bullet"));

	/* Mesh */
	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletMesh"));
	BulletMesh->SetupAttachment(RootComponent);

	ConstructorHelpers::FObjectFinder<UStaticMesh> tmpMesh(TEXT("/Script/Engine.StaticMesh'/Engine/EditorMeshes/ArcadeEditorSphere.ArcadeEditorSphere'"));
	if (tmpMesh.Succeeded())
	{
		BulletMesh->SetStaticMesh(tmpMesh.Object);
		BulletMesh->SetRelativeScale3D(FVector(0.2f));
		BulletMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	else UE_LOG(LogTemp, Warning, TEXT("Failed to load Bullet Mesh"));
}

void ACBullet::BeginPlay()
{
	Super::BeginPlay();
}

void ACBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(bCanMove) MoveBullet(DeltaTime);
}

void ACBullet::ActivateBullet(bool bIsActivate)
{
	// Visibility
	BulletComp->SetVisibility(bIsActivate);

	// Collision
	auto collision = bIsActivate ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision;
	BulletComp->SetCollisionEnabled(collision);

	UE_LOG(LogTemp, Warning, TEXT("Visibility : %d / Collision : %d"), BulletComp->IsVisible(), collision);
}

void ACBullet::MoveBullet(float InDeltaTime)
{
	FVector direction = FireDestination - GetActorLocation();
	SetActorLocation(GetActorLocation() + (direction * BulletSpeed) * InDeltaTime);
}

void ACBullet::Die()
{
}

