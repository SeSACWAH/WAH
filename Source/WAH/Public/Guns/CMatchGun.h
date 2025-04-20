// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Guns/CGun.h"
#include "CMatchGun.generated.h"

/**
 * 
 */
UCLASS()
class WAH_API ACMatchGun : public ACGun
{
	GENERATED_BODY()
	
public:
	ACMatchGun();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

protected:
    UPROPERTY(EditAnywhere, Category = "Gun|Default")
	class USkeletalMeshComponent* GunMeshComp;

	virtual void AddBulletToPool(bool bIsActivate) override;
	UFUNCTION(Server, Reliable)
	void ServerRPC_AddBulletToPool(bool bIsActivate);
	virtual void InitializeBulletPool() override;
	UFUNCTION(Server, Reliable)
	void ServerRPC_InitializeBulletPool();

public:
	UPROPERTY(EditDefaultsOnly, Category = "Gun|Bullet")
	TArray<class ACMatchBullet*> BulletPool;

	UPROPERTY(EditDefaultsOnly, Category = "Gun|Bullet")
	TSubclassOf<class ACMatchBullet> BulletSpawner;

	virtual USkeletalMeshComponent* GetGunMeshComp() override { return GunMeshComp; };
	FVector GetFirePosition();
	virtual void FireBullet(FVector InDestination) override;
	UFUNCTION(Server, Reliable)
	void ServerRPC_FireBullet(FVector InDestination);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_FireBullet(ACMatchBullet* InBullet, FVector InDestination, FVector InFirePosition);

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Gun|FX")
    TArray<class UNiagaraSystem*> FireFXSystemPool;

	UPROPERTY(EditDefaultsOnly, Category = "Gun|FX")
	TArray<class UNiagaraComponent*> FireFXPool;

	virtual void AddFireFXToPool(bool bIsActivate) override;
	virtual void InitializeFireFXPool() override;
	virtual void PlayFireFX() override;
	virtual void OnFireFXFinished(class UNiagaraComponent* InComp) override;
};
