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
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FireTime = 1;

	float CurFireTime = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SapGravity = 500;


	FVector SapVelocity;

	virtual void DoMoveBullet(float InDeltaTime) override;

};
