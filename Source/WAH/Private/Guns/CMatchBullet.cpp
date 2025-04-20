#include "Guns/CMatchBullet.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Components/SphereComponent.h"
#include "../../../../../../../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h"

ACMatchBullet::ACMatchBullet()
{
	BulletComp->OnComponentBeginOverlap.AddDynamic(this, &ACMatchBullet::OnMatchBulletOverlap);
    BulletComp->SetCollisionProfileName(TEXT("Match"));

	ConstructorHelpers::FObjectFinder<UStaticMesh> tmpMesh(TEXT("/Script/Engine.StaticMesh'/Engine/EditorMeshes/ArcadeEditorSphere.ArcadeEditorSphere'"));
	if (tmpMesh.Succeeded())
	{
		BulletMesh->SetStaticMesh(tmpMesh.Object);
		BulletMesh->SetRelativeScale3D(FVector(0.2f));
		BulletMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	else UE_LOG(LogTemp, Warning, TEXT("Failed to load Bullet Mesh"));
}

void ACMatchBullet::OnMatchBulletOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bCanMove) return;

	if (HasAuthority())
	{
		CompleteMoveBullet(SweepResult.Location);
	}
}

void ACMatchBullet::ActivateBullet(bool bIsActivate)
{
	ServerRPC_ActivateBullet(bIsActivate);
	//BulletMesh->SetVisibility(bIsActivate);

	//auto collision = bIsActivate ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision;
	//BulletComp->SetCollisionEnabled(collision);
	////UE_LOG(LogTemp, Warning, TEXT("Visibility : %d / Collision : %d"), BulletComp->IsVisible(), collision);
}

void ACMatchBullet::ServerRPC_ActivateBullet_Implementation(bool bIsActivate)
{
	BulletMesh->SetVisibility(bIsActivate);

	auto collision = bIsActivate ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision;
	BulletComp->SetCollisionEnabled(collision);
	//UE_LOG(LogTemp, Warning, TEXT("Visibility : %d / Collision : %d"), BulletComp->IsVisible(), collision);
}

void ACMatchBullet::DoMoveBullet(float InDeltaTime)
{
	FVector direction = FireDestination - GetActorLocation();
	SetActorLocation(GetActorLocation() + (direction.GetSafeNormal() * BulletSpeed) * InDeltaTime);

	if (FVector::Dist(GetActorLocation(), FireDestination) <= 30)
		CompleteMoveBullet(FireDestination);
}

void ACMatchBullet::CompleteMoveBullet(FVector InDestination)
{
    if (!bCanMove) return;

	ServerRPC_CompleteMoveBullet(InDestination);
    //bCanMove = false;
    //SetActorLocation(InDestination);

    //auto lambda = [&]() {
    //    this->BulletMesh->SetVisibility(false);
    //    GetWorld()->GetTimerManager().ClearTimer(DeactivateTimer);
    //    };
    //GetWorld()->GetTimerManager().SetTimer(DeactivateTimer, lambda, BulletDieTime, false);
}

void ACMatchBullet::ServerRPC_CompleteMoveBullet_Implementation(FVector InDestination)
{
	bCanMove = false;
	SetActorLocation(InDestination);

	auto lambda = [&]() {
		this->BulletMesh->SetVisibility(false);
		GetWorld()->GetTimerManager().ClearTimer(DeactivateTimer);
		};
	GetWorld()->GetTimerManager().SetTimer(DeactivateTimer, lambda, BulletDieTime, false);
}

