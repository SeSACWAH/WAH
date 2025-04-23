// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CWAHGameMode.generated.h"

/**
 * 
 */
UCLASS()
class WAH_API ACWAHGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Class")
    TSubclassOf<APawn> ClassCody;

    UPROPERTY(EditDefaultsOnly, Category = "Class")
    TSubclassOf<APawn> ClassMay;

public:
    ACWAHGameMode();

public:
    UFUNCTION()
    void SpawnPlayerCharacter(AController* Controller);
};
