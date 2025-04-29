// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "enemy/CGiantBeetleFSM.h"
#include "CBeetleAnim.generated.h"

/**
 * 
 */
UCLASS()
class WAH_API UCBeetleAnim : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = FSM)
	EBeetleState AnimState;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = FSM)
	bool IsKill = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = FSM)
	bool bChargeEnd = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = FSM)
	bool bJumpEnd = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = FSM)
	UAnimMontage* DamageAnim;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = FSM)
	UAnimMontage* DieAnim;

	UFUNCTION()
	void AnimNotify_Stomp();

	void PlayDamagedMontage();
	void PlayDieMontage();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
