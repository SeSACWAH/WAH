// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/CMayAnim.h"
#include "Player/CMay.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/GameFramework/CharacterMovementComponent.h"

void UCMayAnim::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    May = Cast<ACMay>(TryGetPawnOwner());
}

void UCMayAnim::NativeUpdateAnimation(float InDeltaSeconds)
{
    Super::NativeUpdateAnimation(InDeltaSeconds);

    if (May)
    {
        Velocity = May->GetVelocity();
        Speed = FVector::DotProduct(May->GetVelocity(), May->GetActorForwardVector());
        Direction = FVector::DotProduct(May->GetVelocity(), May->GetActorRightVector());
        bCanAim = May->GetCanAim();

        bIsFalling = May->bIsFalling;
        JumpCurrentCount = May->PlayerJumpCurrentCount;

        if (JumpCurrentCount >= 1)
        {
            if (May->HasAuthority())
            {
                UE_LOG(LogTemp, Warning, TEXT("SERVER : %d / %d"), bIsFalling, JumpCurrentCount);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("CLIENT : %d / %d"), bIsFalling, JumpCurrentCount);
            }
        }
    }
}

void UCMayAnim::PlayDashAnimation()
{
    UE_LOG(LogTemp, Warning, TEXT("MAY DASH>>> Play Montage Entered"));
    if (DashMontage)
    {
        Montage_Play(DashMontage);
        UE_LOG(LogTemp, Warning, TEXT("MAY DASH>>> Play Montage Completed"));
    }
}
