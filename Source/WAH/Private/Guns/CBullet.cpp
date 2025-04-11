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

	// ���������� ������ �ӷ����� ���ư���
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

