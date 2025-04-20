#include "Player/CPlayer.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "UI/CUnlockedCrossHairUI.h"
#include "../../../../../../../Source/Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "UI/CLockedCrossHairUI.h"
#include "Guns/CGun.h"
#include <Kismet/KismetSystemLibrary.h>
#include "Kismet/GameplayStatics.h"
#include "../../../../../../../Source/Runtime/UMG/Public/Blueprint/WidgetLayoutLibrary.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Components/SphereComponent.h"
#include "EngineUtils.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"

ACPlayer::ACPlayer()
{
    PrimaryActorTick.bCanEverTick = true;

    /* Network */
    bReplicates = true;

    /* Controller Rotation */
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    /* Character Movement */
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->bUseControllerDesiredRotation = true;
    GetCharacterMovement()->MaxWalkSpeed = SpeedJog;

    /* Jump */
    JumpMaxCount = 2;

    /* Camera Boom */
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = ArmLengthDefault;
    CameraBoom->bUsePawnControlRotation = true;
    CameraBoom->SetRelativeLocation(FVector(0, 0, 140));

    /* Camera */
    PlayerCamear = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
    PlayerCamear->SetupAttachment(CameraBoom/*, USpringArmComponent::SocketName*/);
    PlayerCamear->SetRelativeRotation( FRotator(-10, 0, 0) );
    PlayerCamear->bUsePawnControlRotation = false;

    /* Collision */
    GetCapsuleComponent()->SetCollisionProfileName(FName("Player"));
    GetMesh()->SetCollisionProfileName(FName("NoCollision"));

    /* Location */
    GetCapsuleComponent()->SetRelativeLocation(FVector(0, 0, -90));

    /* IMC */
    ConstructorHelpers::FObjectFinder<UInputMappingContext> tmpIMC(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/DYL/Inputs/IMC_Player.IMC_Player'"));
    if (tmpIMC.Succeeded()) IMC_Player = tmpIMC.Object;

    /* IA */
    ConstructorHelpers::FObjectFinder<UInputAction> tmpIAMove(TEXT("/Script/EnhancedInput.InputAction'/Game/DYL/Inputs/IA_Move.IA_Move'"));
    if (tmpIAMove.Succeeded()) IA_Move = tmpIAMove.Object;

    ConstructorHelpers::FObjectFinder<UInputAction> tmpIATurn(TEXT("/Script/EnhancedInput.InputAction'/Game/DYL/Inputs/IA_Turn.IA_Turn'"));
    if (tmpIATurn.Succeeded()) IA_Turn = tmpIATurn.Object;

    ConstructorHelpers::FObjectFinder<UInputAction> tmpIAJump(TEXT("/Script/EnhancedInput.InputAction'/Game/DYL/Inputs/IA_Jump.IA_Jump'"));
    if (tmpIATurn.Succeeded()) IA_Jump = tmpIAJump.Object;

    ConstructorHelpers::FObjectFinder<UInputAction> tmpIARun(TEXT("/Script/EnhancedInput.InputAction'/Game/DYL/Inputs/IA_Run.IA_Run'"));
    if (tmpIARun.Succeeded()) IA_Run = tmpIARun.Object;

    ConstructorHelpers::FObjectFinder<UInputAction> tmpIADash(TEXT("/Script/EnhancedInput.InputAction'/Game/DYL/Inputs/IA_Dash.IA_Dash'"));
    if (tmpIADash.Succeeded()) IA_Dash = tmpIADash.Object;

    ConstructorHelpers::FObjectFinder<UInputAction> tmpIAAim(TEXT("/Script/EnhancedInput.InputAction'/Game/DYL/Inputs/IA_Aim.IA_Aim'"));
    if (tmpIAAim.Succeeded()) IA_Aim = tmpIAAim.Object;

    ConstructorHelpers::FObjectFinder<UInputAction> tmpIAFire(TEXT("/Script/EnhancedInput.InputAction'/Game/DYL/Inputs/IA_Fire.IA_Fire'"));
    if(tmpIAFire.Succeeded()) IA_Fire = tmpIAFire.Object;

    ConstructorHelpers::FObjectFinder<UInputAction> tmpIARevival(TEXT("/Script/EnhancedInput.InputAction'/Game/DYL/Inputs/IA_Revival.IA_Revival'"));
    if (tmpIARevival.Succeeded()) IA_Revival = tmpIARevival.Object;

    // TEST
    ConstructorHelpers::FObjectFinder<UInputAction> tmpTestD(TEXT("/Script/EnhancedInput.InputAction'/Game/DYL/Inputs/IA_TestDamage.IA_TestDamage'"));
    if (tmpTestD.Succeeded()) IA_TestDamage = tmpTestD.Object;

    ConstructorHelpers::FObjectFinder<UInputAction> tmpTestR(TEXT("/Script/EnhancedInput.InputAction'/Game/DYL/Inputs/IA_TestRevival.IA_TestRevival'"));
    if (tmpTestR.Succeeded()) IA_TestRevival = tmpTestR.Object;
}

void ACPlayer::BeginPlay()
{
    Super::BeginPlay();

    /*InitCrosshairWidgets();*/
    // Override
    //AttachGun();
}

void ACPlayer::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // TEST
    PrintNetLog();

    if (HasAuthority())
    {
        // Dash
        if (bCanDash) DoDash(DeltaTime);
        if (bCanResetDash) ResetDash(DeltaTime);

        // Revive
        if(bIsReviving) OnRevive(DeltaTime);

        // God Mode
        if(bIsGodMode) GodMode(DeltaTime);
    }

    if (IsLocallyControlled())
    {
        // Aim
        AdjustTargetArmLocation(DeltaTime);
    }
}

void ACPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // IMC
    APlayerController* pc = Cast<APlayerController>(GetController());
    if (pc)
    {
        auto localPlayer = pc->GetLocalPlayer();
        auto ss = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(localPlayer);
        if (ss) ss->AddMappingContext(IMC_Player, 0);
    }

    // IA
    auto inputSystem = Cast<UEnhancedInputComponent>(PlayerInputComponent);
    if (inputSystem)
    {
        inputSystem->BindAction(IA_Move, ETriggerEvent::Triggered, this, &ACPlayer::DoMove);
        inputSystem->BindAction(IA_Turn, ETriggerEvent::Triggered, this, &ACPlayer::DoTurn);
        inputSystem->BindAction(IA_Jump, ETriggerEvent::Started, this, &ACPlayer::DoJump);
        inputSystem->BindAction(IA_Run, ETriggerEvent::Started, this, &ACPlayer::DoRun);
        inputSystem->BindAction(IA_Dash, ETriggerEvent::Started, this, &ACPlayer::StartDash);
        inputSystem->BindAction(IA_Aim, ETriggerEvent::Started, this, &ACPlayer::StartAim);
        inputSystem->BindAction(IA_Aim, ETriggerEvent::Triggered, this, &ACPlayer::TriggerAim);
        inputSystem->BindAction(IA_Aim, ETriggerEvent::Completed, this, &ACPlayer::CompleteAim);
        inputSystem->BindAction(IA_Fire, ETriggerEvent::Started, this, &ACPlayer::DoFire);
        inputSystem->BindAction(IA_Revival, ETriggerEvent::Started, this, &ACPlayer::RevivalInputEntered);

        // TEST
        inputSystem->BindAction(IA_TestDamage, ETriggerEvent::Started, this, &ACPlayer::TestDamage);
        inputSystem->BindAction(IA_TestRevival, ETriggerEvent::Started, this, &ACPlayer::TestRevival);
    }
}

void ACPlayer::OnRep_HP(int32 InDamage)
{
    GEngine->AddOnScreenDebugMessage(0, 2, FColor::Red, FString::Printf(TEXT("[DAMAGED] Player Get DAMAGED : %d"), HP));

    bIsDamaged = true;
    if (HP <= 0)
    {
        OnDead();
        return;
    }

    auto lambda = [&]() {
        GetWorld()->GetTimerManager().ClearTimer(DamageTimer);
        //RecoverHP();
        auto lambda = [&]() {
            GEngine->AddOnScreenDebugMessage(0, 2, FColor::Red, TEXT("[DAMAGED] TIMER END @@@@@@@@@@@@@@"));
            if (HP < MaxHP)
            {
                GEngine->AddOnScreenDebugMessage(0, 2, FColor::Red, FString::Printf(TEXT("[DAMAGED] Current HP : %d"), HP));
                HP++;
            }
            else
            {
                GEngine->AddOnScreenDebugMessage(0, 2, FColor::Red, FString::Printf(TEXT("[DAMAGED] HP RECOVERED !!!: %d"), HP));
                bIsDamaged = false;
                GetWorld()->GetTimerManager().ClearTimer(RecoverTimer);
            }
            };
        GetWorld()->GetTimerManager().SetTimer(RecoverTimer, lambda, RecoverTime, true);
        };
    GetWorld()->GetTimerManager().SetTimer(DamageTimer, lambda, DamageDurationTime, false);
}

void ACPlayer::OnDamaged(int32 InDamage)
{
    if (bIsGodMode || bIsDead) return;
    ServerRPC_SetHP(InDamage);

    /*bIsDamaged = true;
    HP -= InDamage;
    if(HP <= 0)
    {
        OnDead();
        return;
    }

    auto lambda = [&]() {
            GetWorld()->GetTimerManager().ClearTimer(DamageTimer);
            RecoverHP();
        };
    GetWorld()->GetTimerManager().SetTimer(DamageTimer, lambda, DamageDurationTime, false);*/
}

//void ACPlayer::RecoverHP()
//{
//    auto lambda = [&](){
//        if(HP < MaxHP)
//        {
//            HP++;
//        }
//        else
//        {
//            bIsDamaged = false;
//            GetWorld()->GetTimerManager().ClearTimer(RecoverTimer);
//        }
//            };
//    GetWorld()->GetTimerManager().SetTimer(RecoverTimer, lambda, RecoverTime, true);
//}

void ACPlayer::OnDead()
{
    MulticastRPC_Dead();
    //bIsDead = true;

    //GEngine->AddOnScreenDebugMessage(0, 2, FColor::Red, TEXT("[DEAD] Player DEAD!!!!!"));
    //GetMesh()->SetVisibility(false);
    ////Gun->GetGunMeshComp()->SetVisibility(false);
    //SetActorLocation(GetActorLocation() + -GetActorUpVector() * 100);

    //// 죽음 FX의 Visibility를 켠다

    //// 죽음 FX가 끝나면
    //
    //// 화면 채도가 살짝 낮아진다
    //
    //// 화면이 뿌얘진다
    //
    //// 부활 타이머가 시작된다
    //bIsReviving = true;
    //GEngine->AddOnScreenDebugMessage(1, 2, FColor::Red, TEXT("[REVIVAL] Player REVIVE Start"));

    //// 부활 UI가 뜬다
}

void ACPlayer::RevivalInputEntered(const FInputActionValue& InValue)
{
    if(bIsReviving) bIsRevivalInputEntered = true;
}

void ACPlayer::OnRevive(float InDeltaTime)
{
    if (CurrentReviveTime < RevivalTime)
    {
        if (bIsRevivalInputEntered)
        {
            CurrentReviveTime += InDeltaTime * ReviveBoostAmount;
            bIsRevivalInputEntered = false;
        }
        else CurrentReviveTime += InDeltaTime;
    }
    else
    {
        MulticastRPC_Revive();
        //GetMesh()->SetVisibility(true);
        ////Gun->GetGunMeshComp()->SetVisibility(true);
        //
        ////등장 FX Visible 켜기
        //
        //SetActorLocation(FVector(0));
        //SetActorRotation(FRotator(0, 180, 0));

        //bIsReviving = false;
        //bIsDead = false;
        //bIsDamaged = false;
        //HP = MaxHP;

        //CurrentReviveTime = 0;
        //bIsGodMode = true;
    }
}

void ACPlayer::ServerRPC_SetHP_Implementation(float InDamage)
{
    HP -= InDamage;

    if (HasAuthority()) OnRep_HP(InDamage);
}

void ACPlayer::MulticastRPC_Dead_Implementation()
{
    bIsDead = true;

    GEngine->AddOnScreenDebugMessage(0, 2, FColor::Red, TEXT("[DEAD] Player DEAD!!!!!"));
    GetMesh()->SetVisibility(false);
    //Gun->GetGunMeshComp()->SetVisibility(false);
    SetActorLocation(GetActorLocation() + -GetActorUpVector() * 100);

    // 죽음 FX의 Visibility를 켠다

    // 죽음 FX가 끝나면

    // 화면 채도가 살짝 낮아진다

    // 화면이 뿌얘진다

    // 부활 타이머가 시작된다
    bIsReviving = true;
    GEngine->AddOnScreenDebugMessage(1, 2, FColor::Red, TEXT("[REVIVAL] Player REVIVE Start"));

    // 부활 UI가 뜬다
}

void ACPlayer::MulticastRPC_Revive_Implementation()
{
    GEngine->AddOnScreenDebugMessage(1, 2, FColor::Red, TEXT("[REVIVAL] Player REVIVE Complete"));

    GetMesh()->SetVisibility(true);
    //Gun->GetGunMeshComp()->SetVisibility(true);

    //등장 FX Visible 켜기

    SetActorLocation(FVector(0));
    SetActorRotation(FRotator(0, 180, 0));

    bIsReviving = false;
    bIsDead = false;
    bIsDamaged = false;
    HP = MaxHP;

    CurrentReviveTime = 0;
    bIsGodMode = true;
}

void ACPlayer::GodMode(float InDeltaTime)
{
    if(CurrentGodModeTime < GodModeTime) CurrentGodModeTime += InDeltaTime;
    else
    {
        CurrentGodModeTime = 0;
        bIsGodMode = false;
    }
}

void ACPlayer::DoMove(const struct FInputActionValue& InValue)
{
    if (bCanDash || bIsDead || bIsReviving) return;

    FVector2D scale = InValue.Get<FVector2D>();

    AddMovementInput(PlayerCamear->GetForwardVector() * scale.X + PlayerCamear->GetRightVector() * scale.Y);
}

void ACPlayer::DoTurn(const FInputActionValue& InValue)
{
    FVector2d scale = InValue.Get<FVector2d>();

    float mouseSensitivity = (bCanAim) ? MouseSensitivityAim : MouseSensitivityDefault;

    float min = bIsReviving ? MinPitchRevival : MinPitchDefault;
    float max = bIsReviving ? MaxPitchRevival : MaxPitchDefault;

    float pitch = FMath::Clamp(GetController()->GetControlRotation().Pitch + scale.Y * mouseSensitivity, min, max);
    float yaw = GetController()->GetControlRotation().Yaw + scale.X * mouseSensitivity;
    float roll = GetController()->GetControlRotation().Roll* mouseSensitivity;

    GetController()->SetControlRotation(FRotator(pitch, yaw, roll));
}

void ACPlayer::DoJump(const FInputActionValue& InValue)
{
    if (bIsDead || bIsReviving) return;

    Jump();
}

void ACPlayer::DoRun(const FInputActionValue& InValue)
{
    if (bIsDead || bIsReviving) return;

    if (GetCharacterMovement()->MaxWalkSpeed > SpeedJog)
        GetCharacterMovement()->MaxWalkSpeed = SpeedJog;
    else
        GetCharacterMovement()->MaxWalkSpeed = SpeedRun;
}

void ACPlayer::StartDash(const FInputActionValue& InValue)
{
    if (bIsDead || bIsReviving) return;
    if (bCanDash || bCanResetDash) return;

    ServerRPC_StartDash();
    //DashStartPos = GetActorLocation();
    //DashEndPos = GetActorLocation() + GetActorForwardVector() * DashDistance;

    //bCanDash = true;
}

void ACPlayer::ServerRPC_StartDash_Implementation()
{
    DashStartPos = GetActorLocation();
    DashEndPos = GetActorLocation() + GetActorForwardVector() * DashDistance;

    bCanDash = true;
}

void ACPlayer::DoDash(float InDeltaTime)
{
    DashCurrentTime += InDeltaTime;
    float ratio = DashCurrentTime / DashDurationTime;

    SetActorLocation( FMath::Lerp(DashStartPos, DashEndPos, ratio) );

    if (DashCurrentTime >= DashDurationTime)
    {
        SetActorLocation(DashEndPos);
        bCanDash = false;
        DashCurrentTime = 0;
        bCanResetDash = true;
    }
}

void ACPlayer::ResetDash(float InDeltaTime)
{
    DashCurrentTime += InDeltaTime;

    if (DashCurrentTime >= DashCoolDownTime)
    {
        bCanResetDash = false;
        DashCurrentTime = 0;
    }
}

void ACPlayer::InitCrosshairWidgets()
{
    //if (UnlockedCrossshairWidget)
    //{
    //    UnlockedCrossshairUI = Cast<UCUnlockedCrossHairUI>(CreateWidget(GetWorld(), UnlockedCrossshairWidget));
    //    UnlockedCrossshairUI->SetVisibility(ESlateVisibility::Hidden);
    //    UnlockedCrossshairUI->AddToViewport();
    //}

    //if (LockedCrossshairWidget)
    //{
    //    LockedCrossshairUI = Cast<UCLockedCrossHairUI>(CreateWidget(GetWorld(), LockedCrossshairWidget));
    //    LockedCrossshairUI->SetVisibility(ESlateVisibility::Hidden);
    //    LockedCrossshairUI->AddToViewport();
    //}
}

void ACPlayer::SetUnlockedCrosshairVisibility(bool bVisible)
{
    //if (UnlockedCrossshairUI)
    //    UnlockedCrossshairUI->SetVisibility( bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden );
}

void ACPlayer::SetLockedCrosshairVisibility(bool bVisible)
{
    //if (LockedCrossshairUI)
    //    LockedCrossshairUI->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

float ACPlayer::EaseInOutQuad(float InRatio)
{
    return InRatio < 0.5 ? 2 * InRatio * InRatio : 1 - FMath::Pow(-2 * InRatio + 2, 2) / 2;
}

// for Zoom In - Alpha
float ACPlayer::EaseOutExpo(float InRatio)
{
    return InRatio == 1 ? 1 : 1 - FMath::Pow(2, -10 * InRatio);
}

// for Zoom Out - Alpha
float ACPlayer::EaseOutSine(float InRatio)
{
    return FMath::Sin((InRatio * PI) / 2);
}

void ACPlayer::StartAim(const FInputActionValue& InValue)
{
    //if (bIsDead || bIsDamaged || bIsReviving) return;

    //bCanAim = true;

    //if (CameraBoom->GetComponentLocation() == CameraBoomLocationDefault) ZoomCurrentTime = 0;

    //if (UnlockedCrossshairUI) SetUnlockedCrosshairVisibility(true);

    //bUseControllerRotationYaw = true;
    //bCanZoom = true;
}

void ACPlayer::AdjustTargetArmLocation(float InDeltaTime)
{
    ////UE_LOG(LogTemp, Warning, TEXT(">>> Start Adjust Target Arm Location <<<"));
    //float ratio;

    //if (bCanZoom) ZoomCurrentTime += InDeltaTime;
    //else ZoomCurrentTime -= InDeltaTime;

    //// CurTime이 범위를 벗어나는 것을 한정해줌
    //ZoomCurrentTime = FMath::Clamp(ZoomCurrentTime, 0, AimZoomMaxTime);

    //ratio = EaseInOutQuad(ZoomCurrentTime / AimZoomMaxTime);
    //CameraBoom->SetRelativeLocation(FMath::Lerp(CameraBoomLocationDefault, CameraBoomLocationZoomIn, ratio));
}

void ACPlayer::TriggerAim(const FInputActionValue& InValue)
{
    //if (bIsDead || bIsDamaged || bIsReviving) return;

    //FVector startPos = Gun->GetFirePosition();
    //FVector endPos = startPos + PlayerCamear->GetForwardVector() * SphereTraceDistance;

    //FHitResult hitResult;
    //TArray<AActor*> actorsToIgnore;
    //actorsToIgnore.Add(this);

    //bool bHit = UKismetSystemLibrary::SphereTraceSingle(this, startPos, endPos, SphereTraceRadius, UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel6), false, actorsToIgnore, EDrawDebugTrace::ForDuration, hitResult, true, FColor::Purple, FColor::Orange, 0.3f);
    //
    //if (bHit)
    //{
    //    FireDestination = hitResult.Location;

    //    bool bHitBySap = hitResult.GetComponent()->ComponentHasTag(FName("Sap"));

    //    //UE_LOG(LogTemp, Warning, TEXT("[HIT] bHitBySap : %d / bHitBySapCenter : %d"), bHitBySap, bHitBySapCenter);

    //    // Sap이 들어있는 통에 닿았다면
    //    if (bHitBySap)
    //    {
    //        SetUnlockedCrosshairVisibility(false);
    //        SetLockedCrosshairVisibility(true);

    //        FVector sapCenterLocation;
    //        USphereComponent* sapFull = Cast<USphereComponent>(hitResult.GetComponent());
    //        if (sapFull)
    //        {
    //            sapCenterLocation = sapFull->GetComponentLocation() + sapFull->GetUpVector() * sapFull->GetScaledSphereRadius();
    //            FireDestination = sapCenterLocation;
    //            // Sap Center의 스크린 좌표 위치를 구한다
    //            FVector2D screenPos;
    //            APlayerController* pc = Cast<APlayerController>(GetController());
    //            // 마지막 true : DPI Scaling을 무시하겠다
    //            bool bProjected = UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(pc, sapCenterLocation, screenPos, false);

    //            if (bProjected)
    //            {
    //                // LockedCrossHair의 위치를 FireDestination의 스크린 좌표 위치로 갱신해준다
    //                LockedCrossshairUI->UpdateCrosshairPosition(screenPos);
    //            }
    //        }

    //    }
    //    else
    //    {
    //        SetLockedCrosshairVisibility(false);
    //        SetUnlockedCrosshairVisibility(true);
    //    }
    //}
    //else
    //{
    //    FireDestination = endPos;
    //    SetLockedCrosshairVisibility(false);
    //    SetUnlockedCrosshairVisibility(true);
    //}
}


void ACPlayer::CompleteAim(const FInputActionValue& InValue)
{
    //SetUnlockedCrosshairVisibility(false);
    //SetLockedCrosshairVisibility(false);

    //bUseControllerRotationYaw = false;
    //bCanZoom = false;
    //bCanAim = false;
}


void ACPlayer::AttachGun()
{
    //if (GunBP)
    //{
    //    Gun = GetWorld()->SpawnActor<ACGun>(GunBP);
    //    if(Gun) Gun->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("GunSocket"));
    //    //UE_LOG(LogTemp, Error, TEXT(">>> Attach Gun Success"));
    //}
}

void ACPlayer::DoFire()
{
    //if (bIsDead || bIsReviving) return;

    //// Aim 모드가 아니거나, 총알이 전부 소진되었거나, Fire 중이라면
    //// MAY
    //if(!bCanAim || CurrentBulletCount == 0 || bIsInFireDelayTime) return;
    ////// CODY
    ////if (!bCanAim || CurrentSapAmout <= 0 || bIsInFireDelayTime) return;

    //UE_LOG(LogTemp, Error, TEXT(">>>>> Fire Input Entered <<<<<"));
    //// MAY
    //CurrentBulletCount--;
    //Gun->FireBullet(FireDestination);
    //bIsInFireDelayTime = true;
    //UE_LOG(LogTemp, Error, TEXT(">>> Current Bullet : %d"), CurrentBulletCount);

    //// Fire Delay Time 동안에는 Fire 불가
    //FTimerHandle fireDelayTimer;
    //auto fireDelayLambda = [&]() { bIsInFireDelayTime = false; };
    //GetWorld()->GetTimerManager().SetTimer(fireDelayTimer, fireDelayLambda, FireDelayTime, false);

    //// MAY
    //// 일정 시간이 지나면 Ammo 자동 충전
    //FTimerHandle chargeAmmoTimer;
    //auto chargeAmmoLambda = [&]() {
    //        if(CurrentBulletCount >= MaxBulletCount) return;
    //        CurrentBulletCount++;
    //        UE_LOG(LogTemp, Error, TEXT(">>> Current Bullet : %d"), CurrentBulletCount);
    //    };
    //GetWorld()->GetTimerManager().SetTimer(chargeAmmoTimer, chargeAmmoLambda, ChargeAmmoTime, false);
}

void ACPlayer::PrintNetLog()
{
    const FString ownerName = GetOwner() != nullptr ? GetOwner()->GetName() : TEXT("No Owner");
    FString myLog = FString::Printf(TEXT("%s HP : %d"), *ownerName, HP);

    DrawDebugString(GetWorld(), GetActorLocation() + FVector::UpVector * 100.0f, myLog, nullptr, FColor::White, 0, true);
}

void ACPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    //DOREPLIFETIME(ACPlayer, );
    DOREPLIFETIME(ACPlayer, HP);
    DOREPLIFETIME(ACPlayer, bCanDash);
    DOREPLIFETIME(ACPlayer, bCanResetDash);
}

#pragma region TEST
void ACPlayer::TestDamage(const FInputActionValue& InValue)
{
    UE_LOG(LogTemp, Warning, TEXT("[DAMAGE TEST] DAMAGED!!! Current HP : %d"), 6);
    OnDamaged(6);
}

void ACPlayer::TestRevival(const FInputActionValue& InValue)
{
    UE_LOG(LogTemp, Warning, TEXT("[REVIVAL TEST] DEAD!!! Current HP : %d"), 0);
    OnDamaged(12);
}
#pragma endregion