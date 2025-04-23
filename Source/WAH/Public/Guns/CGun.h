#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CGun.generated.h"

UCLASS()
class WAH_API ACGun : public AActor
{
	GENERATED_BODY()
	
public:	
	ACGun();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

protected:
	//// Gun Mesh
 //   UPROPERTY(EditDefaultsOnly, Category = "Gun|Default")
	//class USkeletalMeshComponent* GunMeshComp;

	// Bullet
	UPROPERTY(EditDefaultsOnly, Category = "Gun|Bullet")
	int32 MaxBulletCnt = 1;

	//UPROPERTY(EditDefaultsOnly, Category = "Gun|Bullet")
	//TArray<class ACBullet*> BulletPool;

	//UPROPERTY(EditDefaultsOnly, Category = "Gun|Bullet")
	//TSubclassOf<class ACBullet> BulletSpawner;

	virtual void AddBulletToPool(bool bIsActivate);
	virtual void InitializeBulletPool();

public:
	virtual USkeletalMeshComponent* GetGunMeshComp();
	virtual void FireBullet(FVector InDestination);

	// Spawn Effect
protected:
	int32 MaxFireFXCnt = 6;

 //   UPROPERTY(EditDefaultsOnly, Category = "Gun|FX")
 //   TArray<class UNiagaraSystem*> FireFXSystemPool;

	//UPROPERTY(EditDefaultsOnly, Category = "Gun|FX")
	//TArray<class UNiagaraComponent*> FireFXPool;

	virtual void AddFireFXToPool(bool bIsActivate);
	virtual void InitializeFireFXPool();
	virtual void PlayFireFX();
	virtual void OnFireFXFinished(class UNiagaraComponent* InComp);

	// Sound
protected:
	// 발사되었을 때 재생할 소리
	UPROPERTY(EditAnywhere, Category = "Gun|Sound")
	class USoundBase* FireSound;

	// Enemy 외의 곳에 충돌했을 때 재생할 소리
	UPROPERTY(EditAnywhere, Category = "Gun|Sound")
	class USoundBase* HitSound;
};
