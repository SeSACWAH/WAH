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

private:
	// Gun Mesh
    UPROPERTY(EditDefaultsOnly, Category = "Gun|Default")
	class UStaticMeshComponent* GunMeshComp;

	// Bullet
	UPROPERTY(EditDefaultsOnly, Category = "Gun|Bullet")
	int32 MaxBulletCnt = 6;

	UPROPERTY(EditDefaultsOnly, Category = "Gun|Bullet")
	TArray<class ACBullet*> BulletPool;

	UPROPERTY(EditDefaultsOnly, Category = "Gun|Bullet")
	TSubclassOf<class ACBullet> BulletSpawner;

	void AddBulletToPool(bool bIsActivate);
	void InitializeBulletPool();
public:
	UStaticMeshComponent* GetGunMeshComp(){return GunMeshComp;};
	FVector GetFirePosition();
	void FireBullet(FVector InDestination);

	// Spawn Effect
private:
	int32 MaxFireFXCnt = 6;

    UPROPERTY(EditDefaultsOnly, Category = "Gun|FX")
    TArray<class UNiagaraSystem*> FireFXSystemPool;

	UPROPERTY(EditDefaultsOnly, Category = "Gun|FX")
	TArray<class UNiagaraComponent*> FireFXPool;


	void AddFireFXToPool(bool bIsActivate);
	void InitializeFireFXPool();
	void PlayFireFX();
	void OnFireFXFinished(UNiagaraComponent* InComp);

	// Sound
private:
	// 발사되었을 때 재생할 소리
	UPROPERTY(EditAnywhere, Category = "Gun|Sound")
	class USoundBase* FireSound;

	// Enemy 외의 곳에 충돌했을 때 재생할 소리
	UPROPERTY(EditAnywhere, Category = "Gun|Sound")
	class USoundBase* HitSound;
};
