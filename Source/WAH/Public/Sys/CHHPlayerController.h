// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CHHPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class WAH_API ACHHPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category=ChooseCharacter)
	TSubclassOf<class APawn> CodyFactory;
	
	UFUNCTION(Server, Reliable)
	void ServerRPC_ChangePlayer(bool bTypeCody);
};
