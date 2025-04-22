#include "Sap/CSapFull.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Components/SphereComponent.h"
#include "Guns/CBullet.h"
#include "Guns/CMatchBullet.h"

ACSapFull::ACSapFull()
{
	PrimaryActorTick.bCanEverTick = true;

	/* Network */
	bReplicates = true;

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
	ACMatchBullet* matchBullet = Cast<ACMatchBullet>(OtherActor);

	if (matchBullet)
	{
		ServerRPC_MatchBulletOverlap(matchBullet);
		//UE_LOG(LogTemp, Warning, TEXT("--- Match Sap overlap ---"));
		//// Destroy ȿ���� �߻��Ѵ�

		//// Bullet Mesh�� Visibility�� ������
		//matchBullet->GetBulletMesh()->SetVisibility(false);

		//// Bullet�� ��ġ�� �ٲ۴�
		//// Visibility�� ���ϱ� Sap�� �پ Collision ������ �ȵ�
		//matchBullet->SetActorLocation(FVector(0, -100, 0));

		//// SapFull�� ��ġ�� ��������

		//// SapFullMesh�� Visibility�� ������
		//SapMeshComp->SetVisibility(false);
	}
}

void ACSapFull::ServerRPC_MatchBulletOverlap_Implementation(class ACMatchBullet* InMatchBullet)
{
	UE_LOG(LogTemp, Warning, TEXT("--- Match Sap overlap ---"));
	// Destroy ȿ���� �߻��Ѵ�

	// Bullet Mesh�� Deactivate�Ѵ�
	InMatchBullet->ActivateBullet(false);

	// Bullet�� ��ġ�� �ٲ۴�
	// Visibility�� ���ϱ� Sap�� �پ Collision ������ �ȵ�
	InMatchBullet->SetActorLocation(FVector(0, -100, 0));

	MulticastRPC_AdjustSapFullLocationAndVisibility();
}

void ACSapFull::MulticastRPC_AdjustSapFullLocationAndVisibility_Implementation()
{
	// SapFull�� ��ġ�� ��������
	SapMeshComp->SetRelativeLocation(GetActorLocation() + FVector(0, -200, 0));

	// SapFullMesh�� Visibility�� ������
	SapMeshComp->SetVisibility(false);
}

