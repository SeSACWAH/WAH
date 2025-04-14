#include "Guns/CGun.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "../../../../../../../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraFunctionLibrary.h"
#include "../../../../../../../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h"
#include "../../../../../../../Plugins/FX/Niagara/Source/Niagara/Classes/NiagaraSystem.h"
#include "Guns/CBullet.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"

ACGun::ACGun()
{
	PrimaryActorTick.bCanEverTick = true;

	// Gun
	GunMeshComp = CreateDefaultSubobject<UStaticMeshComponent>( TEXT("GunMeshComp") );
	SetRootComponent(GunMeshComp);

	ConstructorHelpers::FObjectFinder<UStaticMesh> tmpMesh(TEXT("/Script/Engine.StaticMesh'/Game/DYL/Meshes/PistolMesh/SM_Pistol.SM_Pistol'"));
	if (tmpMesh.Succeeded()) GunMeshComp->SetStaticMesh(tmpMesh.Object);

	// Fire Niagara
	//InitializeFireFXPool();
}

void ACGun::BeginPlay()
{
	Super::BeginPlay();
	
	InitializeBulletPool();
}

void ACGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FVector ACGun::GetFirePosition()
{
	return GunMeshComp->GetSocketLocation(FName("FirePosition"));
}

void ACGun::AddBulletToPool(bool bIsActivate)
{
	FActorSpawnParameters params;
	// 스폰 시 충돌이 생겨도 제자리에서 스폰되게 하는 기능
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	FTransform firePosition = GunMeshComp->GetSocketTransform(TEXT("FirePosition"));
	UE_LOG(LogTemp, Warning, TEXT(">>>>> Socket Tranform : %s"), *GunMeshComp->GetSocketTransform(TEXT("FirePosition")).ToString());

	ACBullet* bullet = GetWorld()->SpawnActor<ACBullet>(BulletSpawner, firePosition, params);

	bullet->ActivateBullet(bIsActivate);
	bullet->SetCanMove(bIsActivate);
	BulletPool.Add(bullet);

	//UE_LOG(LogTemp, Warning, TEXT("[%d] : Add Bullet To Pool"), BulletPool.Num());
}

void ACGun::InitializeBulletPool()
{
	UE_LOG(LogTemp, Warning, TEXT(">>>>> %d : Initialize Bullet Pool Start <<<<<"), BulletPool.Num());
	for (int32 i = 0; i < MaxBulletCnt; ++i) AddBulletToPool(false);
	UE_LOG(LogTemp, Warning, TEXT(">>>>> %d : Initialize Bullet Pool Complete <<<<<"), BulletPool.Num());
}


void ACGun::FireBullet(FVector InDestination)
{
	bool bIsFound = false;

	FVector firePosition = GunMeshComp->GetSocketLocation(TEXT("FirePosition"));

	UE_LOG(LogTemp, Error, TEXT(">>>>> Fire Pos : %s / InDes : %s<<<<<"), *firePosition.ToString(), *InDestination.ToString());

	for (auto bullet : BulletPool)
	{
		// 비활성화 된 총알이라면
		if(!bullet->GetBulletMesh()->GetVisibleFlag())
		{
			// 활성화하고 총구 위치에 배치한다
			bIsFound = true;

			bullet->ActivateBullet(true);
			//firePosition.SetScale3D(bullet->GetBulletComp()->GetComponentScale());
			bullet->SetActorLocation(firePosition);
			bullet->SetFireDestination(InDestination);
			bullet->SetCanMove(true);

			// 소리를 재생한다
			// UGameplayStatics::PlaySoundAtLocation(GetWorld(), FireSound, firePosition.GetLocation());

			// FX를 재생한다
			// PlayFireFX();

			UE_LOG(LogTemp, Warning, TEXT("--------------SPAWN BULLET--------------"));
			// 반복을 그만한다
			break;
		}
	}

	// 사용 가능한 총알이 없다면 추가로 만들고 스폰한다
	if (!bIsFound) 
	{
		UE_LOG(LogTemp, Warning, TEXT("--------------Couldn't find BULLET--------------"));
		AddBulletToPool(true);
	}
}

void ACGun::AddFireFXToPool(bool bIsActivate)
{
	// Niagara System
	UNiagaraSystem* fireSys = nullptr;
	ConstructorHelpers::FObjectFinder<UNiagaraSystem> tmpNiagara(TEXT(""));
	if (tmpNiagara.Succeeded()) fireSys = tmpNiagara.Object;
	else UE_LOG(LogTemp, Warning, TEXT("Failed to load Niagara System"));

	// Niagara Component
	UNiagaraComponent* fireFXComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("FireFXComp"));

	if (fireSys)
	{
		fireFXComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), fireSys, GunMeshComp->GetSocketLocation(FName("FirePosition")));
		fireFXComp->RegisterComponent();
		bIsActivate ? fireFXComp->Deactivate() : fireFXComp->Activate();
		fireFXComp->OnSystemFinished.AddDynamic(this, &ACGun::OnFireFXFinished);
		FireFXPool.Add(fireFXComp);
	}
	else UE_LOG(LogTemp, Warning, TEXT("Invalid FireFXSystem"));
}

void ACGun::InitializeFireFXPool()
{
	UE_LOG(LogTemp, Warning, TEXT(">>>>> %d : Initialize Fire FX Pool Start <<<<<"), FireFXPool.Num());
	for (int32 i = 0; i < MaxFireFXCnt; i++) AddFireFXToPool(false);
	UE_LOG(LogTemp, Warning, TEXT(">>>>> %d : Initialize Bullet Pool Complete <<<<<"), FireFXPool.Num());
}

void ACGun::PlayFireFX()
{
	bool bIsFound = false;

	for (UNiagaraComponent* fireFXComp : FireFXPool)
	{
		// 비활성화된 FX라면 활성화하고 재생한다
		if (!fireFXComp->IsActive())
		{
			fireFXComp->Activate();
			bIsFound = true;
			break;
		}
	}

	// 사용 가능한 FX가 없다면 추가로 만들고 재생한다
	if(!bIsFound) AddBulletToPool(true);
}

void ACGun::OnFireFXFinished(UNiagaraComponent* InComp)
{
	InComp->Deactivate();
}



