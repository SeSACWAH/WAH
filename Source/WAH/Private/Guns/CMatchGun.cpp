#include "Guns/CMatchGun.h"
#include "Guns/CBullet.h"
#include "../../../../../../../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraFunctionLibrary.h"
#include "../../../../../../../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"
#include "Guns/CMatchBullet.h"
#include "Player/CMay.h"
#include "../../../../../../../Source/Runtime/Engine/Public/Net/UnrealNetwork.h"

ACMatchGun::ACMatchGun()
{
	// Gun
	GunMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMeshComp"));
	SetRootComponent(GunMeshComp);

    ConstructorHelpers::FObjectFinder<USkeletalMesh> tmpMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Net/Match/MatchSniper.MatchSniper'"));
    if (tmpMesh.Succeeded()) GunMeshComp->SetSkeletalMesh(tmpMesh.Object);
}

void ACMatchGun::BeginPlay()
{
    Super::BeginPlay();
	
	//if (HasAuthority())
	//{
	//	InitializeBulletPool();
	//}
}

void ACMatchGun::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}


void ACMatchGun::AddBulletToPool(bool bIsActivate)
{
	FActorSpawnParameters params;
	// 스폰 시 충돌이 생겨도 제자리에서 스폰되게 하는 기능
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	// 총알 소유자를 총으로 명시
	if (ACMay* may = Cast<ACMay>(GetOwner()))
	{
		params.Owner = may;
	}

	if (GunMeshComp)
	{
		FTransform firePosition = GunMeshComp->GetSocketTransform(TEXT("FirePosition"));
		//UE_LOG(LogTemp, Warning, TEXT(">>>>> Socket Tranform : %s"), *GunMeshComp->GetSocketTransform(TEXT("FirePosition")).ToString());

		ACMatchBullet* bullet = GetWorld()->SpawnActor<ACMatchBullet>(BulletSpawner, firePosition, params);

		bullet->ActivateBullet(bIsActivate);
		bullet->SetCanMove(bIsActivate);
		BulletPool.Add(bullet);

		//UE_LOG(LogTemp, Warning, TEXT("[%d] : Add Bullet To Pool"), BulletPool.Num());
	}
}

void ACMatchGun::InitializeBulletPool()
{
	UE_LOG(LogTemp, Warning, TEXT(">>>>> %d : Initialize Bullet Poolw Start <<<<<"), BulletPool.Num());
	for (int32 i = 0; i < MaxBulletCnt; ++i) AddBulletToPool(false);
	UE_LOG(LogTemp, Warning, TEXT(">>>>> %d : Initialize Bullet Pool Complete <<<<<"), BulletPool.Num());
}

FVector ACMatchGun::GetFirePosition()
{
	return GunMeshComp->GetSocketLocation(FName("FirePosition"));
}

void ACMatchGun::FireBullet(FVector InDestination)
{
	ServerRPC_FireBullet(InDestination);
	//bool bIsFound = false;

	//FVector firePosition = GunMeshComp->GetSocketLocation(TEXT("FirePosition"));

	//UE_LOG(LogTemp, Error, TEXT(">>>>> Fire Pos : %s / InDes : %s<<<<<"), *firePosition.ToString(), *InDestination.ToString());

	//for (auto bullet : BulletPool)
	//{
	//	// 비활성화 된 총알이라면
	//	if (!bullet->GetBulletMesh()->GetVisibleFlag())
	//	{
	//		// 활성화하고 총구 위치에 배치한다
	//		bIsFound = true;

	//		bullet->ActivateBullet(true);
	//		//firePosition.SetScale3D(bullet->GetBulletComp()->GetComponentScale());
	//		bullet->SetActorLocation(firePosition);
	//		bullet->SetFireDestination(InDestination);
	//		bullet->SetCanMove(true);

	//		// 소리를 재생한다
	//		// UGameplayStatics::PlaySoundAtLocation(GetWorld(), FireSound, firePosition.GetLocation());

	//		// FX를 재생한다
	//		// PlayFireFX();

	//		//UE_LOG(LogTemp, Warning, TEXT("--------------SPAWN BULLET--------------"));
	//		// 반복을 그만한다
	//		break;
	//	}
	//}

	//// 사용 가능한 총알이 없다면 추가로 만들고 스폰한다
	//if (!bIsFound)
	//{
	//	//UE_LOG(LogTemp, Warning, TEXT("--------------Couldn't find BULLET--------------"));
	//	AddBulletToPool(false);
	//	FireBullet(InDestination);
	//}
}

void ACMatchGun::ServerRPC_FireBullet_Implementation(FVector InDestination)
{
	bool bIsFound = false;

	FVector firePosition = GunMeshComp->GetSocketLocation(TEXT("FirePosition"));

	UE_LOG(LogTemp, Error, TEXT(">>>>> Fire Pos : %s / InDes : %s<<<<<"), *firePosition.ToString(), *InDestination.ToString());

	for (auto bullet : BulletPool)
	{
		// 비활성화 된 총알이라면
		if(bullet->IsHidden())
		//if (!bullet->GetBulletMesh()->GetVisibleFlag())
		{
			// 활성화하고 총구 위치에 배치한다
			bIsFound = true;

			FoundBullet = bullet;
			//MulticastRPC_FireBullet(FoundBullet, InDestination, firePosition);
			bullet->ActivateBullet(true);
			bullet->SetActorLocation(firePosition);
			bullet->SetFireDestination(InDestination);
			bullet->SetCanMove(true);

			// 소리를 재생한다
			// UGameplayStatics::PlaySoundAtLocation(GetWorld(), FireSound, firePosition.GetLocation());

			// FX를 재생한다
			// PlayFireFX();

			//UE_LOG(LogTemp, Warning, TEXT("--------------SPAWN BULLET--------------"));
			
			//반복을 그만한다
			break;
		}
	}

	// 사용 가능한 총알이 없다면 추가로 만들고 스폰한다
	if (!bIsFound)
	{
		//UE_LOG(LogTemp, Warning, TEXT("--------------Couldn't find BULLET--------------"));
		AddBulletToPool(false);
		FireBullet(InDestination);
	}
}

//void ACMatchGun::MulticastRPC_FireBullet_Implementation(ACMatchBullet* InBullet, FVector InDestination, FVector InFirePosition)
//{
//	FoundBullet = InBullet;
//	FoundBullet->ActivateBullet(true);
//	FoundBullet->SetActorLocation(InFirePosition);
//	FoundBullet->SetFireDestination(InDestination);
//	FoundBullet->SetCanMove(true);
//
//	// 소리를 재생한다
//	// UGameplayStatics::PlaySoundAtLocation(GetWorld(), FireSound, firePosition.GetLocation());
//
//	// FX를 재생한다
//	// PlayFireFX();
//
//	//UE_LOG(LogTemp, Warning, TEXT("--------------SPAWN BULLET--------------"));
//}

void ACMatchGun::PostNetInit()
{
	Super::PostNetInit();


}

void ACMatchGun::AddFireFXToPool(bool bIsActivate)
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
		fireFXComp->OnSystemFinished.AddDynamic(this, &ACMatchGun::OnFireFXFinished);
		FireFXPool.Add(fireFXComp);
	}
	else UE_LOG(LogTemp, Warning, TEXT("Invalid FireFXSystem"));
}

void ACMatchGun::InitializeFireFXPool()
{
	UE_LOG(LogTemp, Warning, TEXT(">>>>> %d : Initialize Fire FX Pool Start <<<<<"), FireFXPool.Num());
	for (int32 i = 0; i < MaxFireFXCnt; i++) AddFireFXToPool(false);
	UE_LOG(LogTemp, Warning, TEXT(">>>>> %d : Initialize Bullet Pool Complete <<<<<"), FireFXPool.Num());
}

void ACMatchGun::PlayFireFX()
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
	if (!bIsFound) AddFireFXToPool(true);
}

void ACMatchGun::OnFireFXFinished(class UNiagaraComponent* InComp)
{
	InComp->Deactivate();
}

void ACMatchGun::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(ACMatchGun, );
	DOREPLIFETIME(ACMatchGun, BulletPool);
	DOREPLIFETIME(ACMatchGun, FoundBullet);
}