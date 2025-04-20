#include "Guns/CBullet.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"

ACBullet::ACBullet()
{
	PrimaryActorTick.bCanEverTick = true;

	/* Network */
	bReplicates = true;

	/* Collision*/
	BulletComp = CreateDefaultSubobject<USphereComponent>(TEXT("BulletComp"));
	SetRootComponent(BulletComp);
	BulletComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//BulletComp->OnComponentBeginOverlap.AddDynamic(this, &ACBullet::OnMatchBulletOverlap);
	//// MAY
	//BulletComp->SetCollisionProfileName(TEXT("Match"));


	/* Mesh */
	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletMesh"));
	BulletMesh->SetupAttachment(RootComponent);

	//// May
	//ConstructorHelpers::FObjectFinder<UStaticMesh> tmpMesh(TEXT("/Script/Engine.StaticMesh'/Engine/EditorMeshes/ArcadeEditorSphere.ArcadeEditorSphere'"));
	//if (tmpMesh.Succeeded())
	//{
	//	BulletMesh->SetStaticMesh(tmpMesh.Object);
	//	BulletMesh->SetRelativeScale3D(FVector(0.2f));
	//	BulletMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//}
	//else UE_LOG(LogTemp, Warning, TEXT("Failed to load Bullet Mesh"));
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
	//BulletMesh->SetVisibility(bIsActivate);

	//auto collision = bIsActivate ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision;
	//BulletComp->SetCollisionEnabled(collision);
	////UE_LOG(LogTemp, Warning, TEXT("Visibility : %d / Collision : %d"), BulletComp->IsVisible(), collision);
}

void ACBullet::DoMoveBullet(float InDeltaTime)
{
	//FVector direction = FireDestination - GetActorLocation();
	//SetActorLocation(GetActorLocation() + (direction.GetSafeNormal() * BulletSpeed) * InDeltaTime);
	//
	//if (FVector::Dist(GetActorLocation(), FireDestination) <= 30)
	//	CompleteMoveBullet(FireDestination);
}

void ACBullet::CompleteMoveBullet(FVector InDestination)
{
	//if (!bCanMove) return;

	//bCanMove = false;
	//SetActorLocation(InDestination);

	////May
	//auto lambda = [&]() { 
	//	this->BulletMesh->SetVisibility(false); 
	//	GetWorld()->GetTimerManager().ClearTimer(DeactivateTimer);
	//	};
	//GetWorld()->GetTimerManager().SetTimer(DeactivateTimer, lambda, BulletDieTime, false);
}

//void ACBullet::OnMatchBulletOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//	if(!bCanMove) return;
//
//	CompleteMoveBullet(SweepResult.Location);
//}

void ACBullet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(ACBullet, );
	DOREPLIFETIME(ACBullet, bCanMove);
	DOREPLIFETIME(ACBullet, FireDestination);
}