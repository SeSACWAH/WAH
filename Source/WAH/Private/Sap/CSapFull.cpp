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
	ACMatchBullet* matchBullet = Cast<ACMatchBullet>(OtherActor);

	if (matchBullet)
	{
		ServerRPC_MatchBulletOverlap(matchBullet);
		//UE_LOG(LogTemp, Warning, TEXT("--- Match Sap overlap ---"));
		//// Destroy 효과가 발생한다

		//// Bullet Mesh의 Visibility가 꺼진다
		//matchBullet->GetBulletMesh()->SetVisibility(false);

		//// Bullet의 위치를 바꾼다
		//// Visibility만 끄니까 Sap에 붙어서 Collision 감지가 안됨
		//matchBullet->SetActorLocation(FVector(0, -100, 0));

		//// SapFull의 위치가 내려간다

		//// SapFullMesh의 Visibility가 꺼진다
		//SapMeshComp->SetVisibility(false);
	}
}

void ACSapFull::ServerRPC_MatchBulletOverlap_Implementation(class ACMatchBullet* InMatchBullet)
{
	UE_LOG(LogTemp, Warning, TEXT("--- Match Sap overlap ---"));
	// Destroy 효과가 발생한다

	// Bullet Mesh를 Deactivate한다
	InMatchBullet->ActivateBullet(false);

	// Bullet의 위치를 바꾼다
	// Visibility만 끄니까 Sap에 붙어서 Collision 감지가 안됨
	InMatchBullet->SetActorLocation(FVector(0, -100, 0));

	MulticastRPC_AdjustSapFullLocationAndVisibility();
}

void ACSapFull::MulticastRPC_AdjustSapFullLocationAndVisibility_Implementation()
{
	// SapFull의 위치가 내려간다
	SapMeshComp->SetRelativeLocation(GetActorLocation() + FVector(0, -200, 0));

	// SapFullMesh의 Visibility가 꺼진다
	SapMeshComp->SetVisibility(false);
}

