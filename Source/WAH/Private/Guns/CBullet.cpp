#include "Guns/CBullet.h"
#include "CBullet.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Components/SphereComponent.h"

ACBullet::ACBullet()
{
	PrimaryActorTick.bCanEverTick = true;

	Bullet = CreateDefaultSubobject<USphereComponent>(TEXT("Bullet"));
	Bullet->SetupAttachment(RootComponent);
}

void ACBullet::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACBullet::MoveBullet(FVector InStartPos, FVector InEndPos)
{
	// 입력이 들어올 때마다 실행되므로 비워준다
	Lines.Empty();

	FVector velocity = (InEndPos - InStartPos) * BulletForce;
	FVector pos = InStartPos;

	Lines.Add(pos);

	for (int i = 0; i < LineSmooth - 1; i++)
	{
		FVector lastPos = pos;

		velocity += FVector::UpVector * BulletGravity * BulletSimulateTime;

		pos += velocity * BulletSimulateTime;

	}
}

