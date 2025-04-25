#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CMayAnim.generated.h"

UCLASS()
class WAH_API UCMayAnim : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float InDeltaSeconds) override;

public:
	UPROPERTY()
	class ACMay* May;

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

	UPROPERTY(EditAnywhere, Category = Dash)
	class UAnimMontage* DashMontage;

	void PlayDashAnimation();
};
