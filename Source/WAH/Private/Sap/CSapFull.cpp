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
	
	// ������ �������� �� SapMesh�� Visibility�� ���ش�
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
		// Destroy ȿ���� �߻��Ѵ�

		// Bullet Mesh�� Visibility�� ������
		bullet->GetBulletMesh()->SetVisibility(false);

		// Bullet�� ��ġ�� �ٲ۴�
		// Visibility�� ���ϱ� Sap�� �پ Collision ������ �ȵ�
		bullet->SetActorLocation(FVector(0, -100, 0));

		// SapFullMesh�� Visibility�� ������
		SapMeshComp->SetVisibility(false);

		// SapFull�� ��ġ�� ��������

	}
}