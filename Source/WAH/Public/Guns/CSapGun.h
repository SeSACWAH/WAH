// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Guns/CGun.h"
#include "CSapGun.generated.h"

/**
 * 
 */
UCLASS()
class WAH_API ACSapGun : public ACGun
{
	GENERATED_BODY()
public:
	ACSapGun();

	UPROPERTY(EditDefaultsOnly, Category = "Gun|Default")
	class USkeletalMeshComponent* GunMeshComp;

	UPROPERTY(EditDefaultsOnly, Category = "Gun|Default")
	TSubclassOf<class ACSap> SapFac;

	void Fire();

	virtual void FireBullet(FVector InDestination) override;
};
