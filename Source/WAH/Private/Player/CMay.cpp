#include "Player/CMay.h"
#include "Guns/CGun.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Camera/CameraComponent.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Components/SphereComponent.h"
#include "../../../../../../../Source/Runtime/UMG/Public/Blueprint/WidgetLayoutLibrary.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "UI/CLockedCrossHairUI.h"
#include <Guns/CMatchGun.h>
#include "UI/CUnlockedCrossHairUI.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/GameFramework/SpringArmComponent.h"

ACMay::ACMay()
{
    // Skeleta Mesh
    ConstructorHelpers::FObjectFinder<USkeletalMesh> tmpMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Net/May/May.May'"));
    if(tmpMesh.Succeeded()) GetMesh()->SetSkeletalMesh(tmpMesh.Object);
}

void ACMay::BeginPlay()
{
    Super::BeginPlay();

    InitCrosshairWidgets();
    AttachGun();
}

void ACMay::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ACMay::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ACMay::DoFire()
{
    if (bIsDead || bIsReviving) return;

    // Aim 모드가 아니거나, 총알이 전부 소진되었거나, Fire 중이라면
    if (!bCanAim || CurrentBulletCount == 0 || bIsInFireDelayTime) return;

    // UE_LOG(LogTemp, Error, TEXT(">>>>> Fire Input Entered <<<<<"));
    CurrentBulletCount--;
    MatchGun->FireBullet(FireDestination);
    bIsInFireDelayTime = true;
    // UE_LOG(LogTemp, Error, TEXT(">>> Current Bullet : %d"), CurrentBulletCount);

    // Fire Delay Time 동안에는 Fire 불가
    FTimerHandle fireDelayTimer;
    auto fireDelayLambda = [&]() { bIsInFireDelayTime = false; };
    GetWorld()->GetTimerManager().SetTimer(fireDelayTimer, fireDelayLambda, FireDelayTime, false);

    // 일정 시간이 지나면 Ammo 자동 충전
    FTimerHandle chargeAmmoTimer;
    auto chargeAmmoLambda = [&]() {
        if (CurrentBulletCount >= MaxBulletCount) return;
        CurrentBulletCount++;
        // UE_LOG(LogTemp, Error, TEXT(">>> Current Bullet : %d"), CurrentBulletCount);
        };
    GetWorld()->GetTimerManager().SetTimer(chargeAmmoTimer, chargeAmmoLambda, ChargeAmmoTime, false);
}

void ACMay::AttachGun()
{
    if (MatchBP)
    {
        MatchGun = GetWorld()->SpawnActor<ACMatchGun>(MatchBP);
        if (MatchGun) MatchGun->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("GunSocket"));
        //UE_LOG(LogTemp, Error, TEXT(">>> Attach Gun Success"));
    }
}

void ACMay::InitCrosshairWidgets()
{
    if (UnlockedCrossshairWidget)
    {
        UnlockedCrossshairUI = Cast<UCUnlockedCrossHairUI>(CreateWidget(GetWorld(), UnlockedCrossshairWidget));
        UnlockedCrossshairUI->SetVisibility(ESlateVisibility::Hidden);
        UnlockedCrossshairUI->AddToViewport();
    }

    if (LockedCrossshairWidget)
    {
        LockedCrossshairUI = Cast<UCLockedCrossHairUI>(CreateWidget(GetWorld(), LockedCrossshairWidget));
        LockedCrossshairUI->SetVisibility(ESlateVisibility::Hidden);
        LockedCrossshairUI->AddToViewport();
    }
}

void ACMay::SetUnlockedCrosshairVisibility(bool bVisible)
{
    if (UnlockedCrossshairUI)
        UnlockedCrossshairUI->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void ACMay::SetLockedCrosshairVisibility(bool bVisible)
{
    if (LockedCrossshairUI)
        LockedCrossshairUI->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void ACMay::StartAim(const FInputActionValue& InValue)
{
    if (bIsDead || bIsDamaged || bIsReviving) return;

    bCanAim = true;

    if (CameraBoom->GetComponentLocation() == CameraBoomLocationDefault) ZoomCurrentTime = 0;

    if (UnlockedCrossshairUI) SetUnlockedCrosshairVisibility(true);

    bUseControllerRotationYaw = true;
    bCanZoom = true;
}

void ACMay::AdjustTargetArmLocation(float InDeltaTime)
{
    //UE_LOG(LogTemp, Warning, TEXT(">>> Start Adjust Target Arm Location <<<"));
    float ratio;

    if (bCanZoom) ZoomCurrentTime += InDeltaTime;
    else ZoomCurrentTime -= InDeltaTime;

    // CurTime이 범위를 벗어나는 것을 한정해줌
    ZoomCurrentTime = FMath::Clamp(ZoomCurrentTime, 0, AimZoomMaxTime);

    ratio = EaseInOutQuad(ZoomCurrentTime / AimZoomMaxTime);
    CameraBoom->SetRelativeLocation(FMath::Lerp(CameraBoomLocationDefault, CameraBoomLocationZoomIn, ratio));
}

void ACMay::TriggerAim(const FInputActionValue& InValue)
{
    if (bIsDead || bIsDamaged || bIsReviving) return;

    FVector startPos = MatchGun->GetFirePosition();
    FVector endPos = startPos + PlayerCamear->GetForwardVector() * SphereTraceDistance;

    FHitResult hitResult;
    TArray<AActor*> actorsToIgnore;
    actorsToIgnore.Add(this);

    bool bHit = UKismetSystemLibrary::SphereTraceSingle(this, startPos, endPos, SphereTraceRadius, UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel6), false, actorsToIgnore, EDrawDebugTrace::ForDuration, hitResult, true, FColor::Purple, FColor::Orange, 0.3f);

    if (bHit)
    {
        FireDestination = hitResult.Location;

        bool bHitBySap = hitResult.GetComponent()->ComponentHasTag(FName("Sap"));

        //UE_LOG(LogTemp, Warning, TEXT("[HIT] bHitBySap : %d / bHitBySapCenter : %d"), bHitBySap, bHitBySapCenter);

        // Sap이 들어있는 통에 닿았다면
        if (bHitBySap)
        {
            SetUnlockedCrosshairVisibility(false);
            SetLockedCrosshairVisibility(true);

            FVector sapCenterLocation;
            USphereComponent* sapFull = Cast<USphereComponent>(hitResult.GetComponent());
            if (sapFull)
            {
                sapCenterLocation = sapFull->GetComponentLocation() + sapFull->GetUpVector() * sapFull->GetScaledSphereRadius();
                FireDestination = sapCenterLocation;
                // Sap Center의 스크린 좌표 위치를 구한다
                FVector2D screenPos;
                APlayerController* pc = Cast<APlayerController>(GetController());
                // 마지막 true : DPI Scaling을 무시하겠다
                bool bProjected = UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(pc, sapCenterLocation, screenPos, false);

                if (bProjected)
                {
                    // LockedCrossHair의 위치를 FireDestination의 스크린 좌표 위치로 갱신해준다
                    LockedCrossshairUI->UpdateCrosshairPosition(screenPos);
                }
            }

        }
        else
        {
            SetLockedCrosshairVisibility(false);
            SetUnlockedCrosshairVisibility(true);
        }
    }
    else
    {
        FireDestination = endPos;
        SetLockedCrosshairVisibility(false);
        SetUnlockedCrosshairVisibility(true);
    }
}

void ACMay::CompleteAim(const FInputActionValue& InValue)
{
    SetUnlockedCrosshairVisibility(false);
    SetLockedCrosshairVisibility(false);

    bUseControllerRotationYaw = false;
    bCanZoom = false;
    bCanAim = false;
}

void ACMay::OnDead()
{
    Super::OnDead();
    MatchGun->GetGunMeshComp()->SetVisibility(false);
}

void ACMay::OnRevive(float InDeltaTime)
{
    Super::OnRevive(InDeltaTime);
    if (CurrentReviveTime >= RevivalTime)
    {
        MatchGun->GetGunMeshComp()->SetVisibility(true);

        //등장 FX Visible 켜기
    }
}
