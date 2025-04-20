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
#include "Net/UnrealNetwork.h"

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

    // Aim ��尡 �ƴϰų�, �Ѿ��� ���� �����Ǿ��ų�, Fire ���̶��
    if (!bCanAim || CurrentBulletCount == 0 || bIsInFireDelayTime) return;

    ServerRPC_DoFire();
    //// UE_LOG(LogTemp, Error, TEXT(">>>>> Fire Input Entered <<<<<"));
    //CurrentBulletCount--;
    //MatchGun->FireBullet(FireDestination);
    //bIsInFireDelayTime = true;
    //// UE_LOG(LogTemp, Error, TEXT(">>> Current Bullet : %d"), CurrentBulletCount);

    //// Fire Delay Time ���ȿ��� Fire �Ұ�
    //FTimerHandle fireDelayTimer;
    //auto fireDelayLambda = [&]() { bIsInFireDelayTime = false; };
    //GetWorld()->GetTimerManager().SetTimer(fireDelayTimer, fireDelayLambda, FireDelayTime, false);

    //// ���� �ð��� ������ Ammo �ڵ� ����
    //FTimerHandle chargeAmmoTimer;
    //auto chargeAmmoLambda = [&]() {
    //    if (CurrentBulletCount >= MaxBulletCount) return;
    //    CurrentBulletCount++;
    //    // UE_LOG(LogTemp, Error, TEXT(">>> Current Bullet : %d"), CurrentBulletCount);
    //    };
    //GetWorld()->GetTimerManager().SetTimer(chargeAmmoTimer, chargeAmmoLambda, ChargeAmmoTime, false);
}

void ACMay::ServerRPC_DoFire_Implementation()
{
    // UE_LOG(LogTemp, Error, TEXT(">>>>> Fire Input Entered <<<<<"));
    CurrentBulletCount--;
    MatchGun->FireBullet(FireDestination);
    bIsInFireDelayTime = true;
    // UE_LOG(LogTemp, Error, TEXT(">>> Current Bullet : %d"), CurrentBulletCount);

    // Fire Delay Time ���ȿ��� Fire �Ұ�
    FTimerHandle fireDelayTimer;
    auto fireDelayLambda = [&]() { bIsInFireDelayTime = false; };
    GetWorld()->GetTimerManager().SetTimer(fireDelayTimer, fireDelayLambda, FireDelayTime, false);

    // ���� �ð��� ������ Ammo �ڵ� ����
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

    ServerRPC_StartAim();
    //bUseControllerRotationYaw = true;
    //bCanAim = true;
    //bCanZoom = true;

    if (CameraBoom->GetComponentLocation() == CameraBoomLocationDefault) ZoomCurrentTime = 0;
    if (UnlockedCrossshairUI) SetUnlockedCrosshairVisibility(true);

}

void ACMay::ServerRPC_StartAim_Implementation()
{
    bCanAim = true;
    bCanZoom = true;
    MulticastRPC_StartAim();
}

void ACMay::MulticastRPC_StartAim_Implementation()
{
    bUseControllerRotationYaw = true;
}

void ACMay::AdjustTargetArmLocation(float InDeltaTime)
{
    //UE_LOG(LogTemp, Warning, TEXT(">>> Start Adjust Target Arm Location <<<"));
    float ratio;

    if (bCanZoom) ZoomCurrentTime += InDeltaTime;
    else ZoomCurrentTime -= InDeltaTime;

    // CurTime�� ������ ����� ���� ��������
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

    FVector fireDestination;

    if (bHit)
    {
        fireDestination = hitResult.Location;

        bool bHitBySap = hitResult.GetComponent()->ComponentHasTag(FName("Sap"));

        //UE_LOG(LogTemp, Warning, TEXT("[HIT] bHitBySap : %d / bHitBySapCenter : %d"), bHitBySap, bHitBySapCenter);

        // Sap�� ����ִ� �뿡 ��Ҵٸ�
        if (bHitBySap)
        {
            SetUnlockedCrosshairVisibility(false);
            SetLockedCrosshairVisibility(true);

            FVector sapCenterLocation;
            USphereComponent* sapFull = Cast<USphereComponent>(hitResult.GetComponent());
            if (sapFull)
            {
                sapCenterLocation = sapFull->GetComponentLocation() + sapFull->GetUpVector() * sapFull->GetScaledSphereRadius();
                fireDestination = sapCenterLocation;
                // Sap Center�� ��ũ�� ��ǥ ��ġ�� ���Ѵ�
                FVector2D screenPos;
                APlayerController* pc = Cast<APlayerController>(GetController());
                // ������ true : DPI Scaling�� �����ϰڴ�
                bool bProjected = UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(pc, sapCenterLocation, screenPos, false);

                if (bProjected)
                {
                    // LockedCrossHair�� ��ġ�� FireDestination�� ��ũ�� ��ǥ ��ġ�� �������ش�
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

    ServerRPC_UpdateFireDestination(fireDestination);
}

void ACMay::ServerRPC_UpdateFireDestination_Implementation(FVector InFireDestination)
{
    FireDestination = InFireDestination;
}

void ACMay::CompleteAim(const FInputActionValue& InValue)
{
    SetUnlockedCrosshairVisibility(false);
    SetLockedCrosshairVisibility(false);


    ServerRPC_CompleteAim();
    //bUseControllerRotationYaw = false;
    //bCanAim = false;
    //bCanZoom = false;
}

void ACMay::ServerRPC_CompleteAim_Implementation()
{
    MulticastRPC_CompleteAim();
    bCanAim = false;
    bCanZoom = false;
}

void ACMay::MulticastRPC_CompleteAim_Implementation()
{
    bUseControllerRotationYaw = false;
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

        //���� FX Visible �ѱ�
    }
}

void ACMay::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    //DOREPLIFETIME(ACMay, );
    DOREPLIFETIME(ACMay, CurrentBulletCount);
    DOREPLIFETIME(ACMay, bIsInFireDelayTime);
}
