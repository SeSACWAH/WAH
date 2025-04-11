#include "Guns/CBullet.h"
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

	// 목적지까지 일정한 속력으로 날아간다
	FVector p = StartPos + Velocity * DeltaTime;
	if(FVector::Distance(p, EndPos) >= 0)
		SetActorLocation(p);
}

void ACBullet::SetPositionAndVelocity(FVector InStartPos, FVector InEndPos)
{
	StartPos = InStartPos;
	EndPos = InEndPos;
	Velocity = (InEndPos - InStartPos) * BulletSpeed;
}

