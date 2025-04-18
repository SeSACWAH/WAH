// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Guns/CBullet.h"
#include "CMatchBullet.generated.h"

/**
 * 
 */
UCLASS()
class WAH_API ACMatchBullet : public ACBullet
{
	GENERATED_BODY()
	
public:
	ACMatchBullet();

	UFUNCTION()
	void OnMatchBulletOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	virtual USphereComponent* GetBulletComp() const override { return BulletComp; };
	virtual UStaticMeshComponent* GetBulletMesh() const override { return BulletMesh; }
	virtual void SetFireDestination(FVector InDirection) override { FireDestination = InDirection; }
	virtual void SetCanMove(bool InResult) override { bCanMove = InResult; }

	virtual void ActivateBullet(bool bIsActivate) override;
	virtual void DoMoveBullet(float InDeltaTime) override;
	virtual void CompleteMoveBullet(FVector InDestination) override;
};
