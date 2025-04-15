#include "Sap/CSapFull.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Components/SphereComponent.h"
#include "Guns/CBullet.h"

ACSapFull::ACSapFull()
{
	PrimaryActorTick.bCanEverTick = true;

	/* Sap Comp */
	SapComp = CreateDefaultSubobject<USphereComponent>(TEXT("SapComp"));
	RootComponent = SapComp;
	SapComp->SetSphereRadius(100.f);
	SapComp->SetCollisionProfileName(FName("Sap"));
	SapComp->ComponentTags.Add(FName("Sap"));
	SapComp->OnComponentBeginOverlap.AddDynamic(this, &ACSapFull::OnSapFullOverlap);

	/* Sap Mesh Comp */
	SapMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SapMesh"));
	SapMeshComp->SetupAttachment(RootComponent);
	ConstructorHelpers::FObjectFinder<UStaticMesh> tmpMesh(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	if(tmpMesh.Succeeded()) SapMeshComp->SetStaticMesh(tmpMesh.Object);
	SapMeshComp->SetRelativeScale3D(FVector(2));
	SapMeshComp->SetCollisionProfileName(FName("NoCollision"));
}

void ACSapFull::BeginPlay()
{
	Super::BeginPlay();
	
	// 게임이 시작했을 땐 SapMesh의 Visibility를 꺼준다
	//SapMeshComp->SetVisibility(false);
}

void ACSapFull::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACSapFull::OnSapFullOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("--- Sap overlap ---"));
	ACBullet* bullet = Cast<ACBullet>(OtherActor);

	if (bullet)
	{
		UE_LOG(LogTemp, Warning, TEXT("--- Sap overlap222222 ---"));
		// Destroy 효과가 발생한다

		// Bullet Mesh의 Visibility가 꺼진다
		bullet->GetBulletMesh()->SetVisibility(false);

		// Bullet의 위치를 바꾼다
		// Visibility만 끄니까 Sap에 붙어서 Collision 감지가 안됨
		bullet->SetActorLocation(FVector(0, -100, 0));

		// SapFullMesh의 Visibility가 꺼진다
		SapMeshComp->SetVisibility(false);

		// SapFull의 위치가 내려간다

	}
}