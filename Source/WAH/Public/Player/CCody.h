// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/CPlayer.h"
#include "CCody.generated.h"

/**
 * 
 */
UCLASS()
class WAH_API ACCody : public ACPlayer
{
	GENERATED_BODY()
	
public:

	ACCody();

	virtual void BeginPlay() override;

	FVector FireDest = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class ACGun> GunBP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class ACGun* Gun;

	virtual void DoFire() override;

	UFUNCTION(Server, Reliable)
	void ServerRPC_Fire();

	virtual void OnDead() override;
	UFUNCTION(Server, Reliable)
	void ServerRPC_SetGun(bool bVisible);

	virtual void OnRevive(float InDeltaTime) override;
};

