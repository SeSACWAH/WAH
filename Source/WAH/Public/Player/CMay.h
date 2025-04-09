// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/CPlayer.h"
#include "CMay.generated.h"

/**
 * 
 */
UCLASS()
class WAH_API ACMay : public ACPlayer
{
	GENERATED_BODY()

public:
    ACMay();
	
protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
