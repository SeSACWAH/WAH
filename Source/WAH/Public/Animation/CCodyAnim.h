// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CCodyAnim.generated.h"

/**
 * 
 */
UCLASS()
class WAH_API UCCodyAnim : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

	UPROPERTY()
	class ACCody* Cody;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnimSettings)
	FVector Velocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnimSettings)
	float Direction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnimSettings)
	float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnimSettings)
	bool bIsFalling;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnimSettings)
	bool bWasJumping;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnimSettings)
	int32 JumpCurrentCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnimSettings)
	bool bCanAim = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnimSettings)
	float pitchAngle;

	UPROPERTY(EditAnywhere, Category = Dash)
	class UAnimMontage* DashMontage;

	void PlayDashAnimation();
};
