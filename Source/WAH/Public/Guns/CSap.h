// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Guns/CBullet.h"
#include "CSap.generated.h"

/**
 * 
 */
UCLASS()
class WAH_API ACSap : public ACBullet
{
	GENERATED_BODY()
	
public:
	ACSap();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FireTime = 1;

	float CurFireTime = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SapGravity = 500;

	float SapGather = 0;

	FVector SapVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UProjectileMovementComponent* MoveComp;

	void SetSapGather(float sapGather);

	void Explosion(FVector lot);

	float ExplosionRadius = 300;

	UFUNCTION(Server, Unreliable)
	void ServerRPC_Explosion(FVector lot);

	UFUNCTION(NetMulticast, Unreliable)
	void MultiRPC_Explosion(FVector lot);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UNiagaraSystem* ExplosionSys;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UNiagaraComponent* ExplosionComp;

	UFUNCTION()
    virtual void OnSapOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
