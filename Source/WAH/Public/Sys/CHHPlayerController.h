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

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UCharacterSelectWidget> widgetFac;

	UPROPERTY(EditDefaultsOnly)
	class UCharacterSelectWidget* widget;
	
	UFUNCTION(Server, Reliable)
	void ServerRPC_ChangePlayer(bool bTypeCody);
	/*UFUNCTION(Server, Reliable)
	void ServerRPC_SelectCody(bool bSCody);
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPC_SelectCody(bool bSCody);
	UFUNCTION(Server, Reliable)
	void ServerRPC_SelectMay(bool bSMay);
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPC_SelectMay(bool bSMay);

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	bool bSelectCody = false;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	bool bSelectMay = false;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	FString selectName;

	virtual void OnPossess(APawn* aPawn) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const override;*/
};
