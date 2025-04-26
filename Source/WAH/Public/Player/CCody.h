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
	virtual void Tick(float DeltaTime) override;

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

	virtual void StartAim(const FInputActionValue& InValue) override;

	UFUNCTION(Server, Reliable)
	void ServerRPC_StartAim();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_StartAim();

	virtual void CompleteAim(const FInputActionValue& InValue) override;

	UFUNCTION(Server, Reliable)
	void ServerRPC_CompleteAim();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_CompleteAim();

	UPROPERTY(ReplicatedUsing=OnRep_CameraBoomRotation)
    FRotator CameraBoomRotation;

	UPROPERTY(ReplicatedUsing=OnRep_CameraBoomPosition)
    FVector CameraBoomPosition;

    UFUNCTION()
    void OnRep_CameraBoomRotation();
	UFUNCTION()
    void OnRep_CameraBoomPosition();


    UFUNCTION(NetMulticast, Unreliable)
    void MulticastRPC_UpdateCaptureRotation(FVector loc, FRotator rot);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
};

