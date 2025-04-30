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
	// Bullet
	UPROPERTY(EditDefaultsOnly, Category = "Gun|Bullet")
	int32 MaxBulletCnt = 1;

	virtual void AddBulletToPool(bool bIsActivate);
	virtual void InitializeBulletPool();

public:
	virtual USkeletalMeshComponent* GetGunMeshComp();
	virtual void FireBullet(FVector InDestination);

	// Spawn Effect
protected:
	int32 MaxFireFXCnt = 6;

	virtual void AddFireFXToPool(bool bIsActivate);
	virtual void InitializeFireFXPool();
	virtual void PlayFireFX();
	virtual void OnFireFXFinished(class UNiagaraComponent* InComp);

	// Sound
protected:
	// �߻�Ǿ��� �� ����� �Ҹ�
	UPROPERTY(EditAnywhere, Category = "Gun|Sound")
	class USoundBase* FireSound;

	// Enemy ���� ���� �浹���� �� ����� �Ҹ�
	UPROPERTY(EditAnywhere, Category = "Gun|Sound")
	class USoundBase* HitSound;
};
