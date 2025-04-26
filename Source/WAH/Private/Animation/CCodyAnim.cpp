// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/CCodyAnim.h"
#include "Player/CCody.h"
#include "GameFramework/CharacterMovementComponent.h"

void UCCodyAnim::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	Cody = Cast<ACCody>(TryGetPawnOwner());
}

void UCCodyAnim::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);
	if (Cody)
	{
		Velocity = Cody->GetVelocity();
		Speed = FVector::DotProduct(Velocity,Cody->GetActorForwardVector());
		Direction = FVector::DotProduct(Velocity,Cody->GetActorRightVector());
		bCanAim = Cody->GetCanAim();
		bIsFalling = Cody->GetCharacterMovement()->IsFalling();
		bWasJumping = Cody->bWasJumping;
		JumpCurrentCount = Cody->JumpCurrentCount;

		pitchAngle =  - Cody->GetBaseAimRotation().GetNormalized().Pitch;
	}
}

void UCCodyAnim::PlayDashAnimation()
{
	if (DashMontage)
	{
		Montage_Play(DashMontage);
	}
}
