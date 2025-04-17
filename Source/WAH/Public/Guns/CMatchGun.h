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

	virtual void AddBulletToPool(bool bIsActivate) override;
};
