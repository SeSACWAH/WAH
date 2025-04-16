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

ACPlayer::ACPlayer()
{
    PrimaryActorTick.bCanEverTick = true;

    /* Controller Rotation */
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    /* Character Movement */
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->bUseControllerDesiredRotation = false;
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

    // Crosshair
    InitCrosshairWidgets();

    // Gun
    AttachGun();

    //// Damage Recover Test
    //FTimerHandle testGetDamage;
    //auto lambda1 = [&]()
    //    {
    //        UE_LOG(LogTemp, Warning, TEXT("[DAMAGE TEST] DAMAGED!!! Current HP : %d"), 6);
    //        OnDamaged(6);
    //    };
    //GetWorld()->GetTimerManager().SetTimer(testGetDamage, lambda1, 3.f, false);

    //// Dead Test
    //FTimerHandle testDead;
    //auto lambda2 = [&]()
    //    {
    //        UE_LOG(LogTemp, Warning, TEXT("[DEAD TEST] DEAD!!! Current HP : %d"), 0);
    //        OnDamaged(12);
    //    };
    //GetWorld()->GetTimerManager().SetTimer(testDead, lambda2, 20.f, false);

    //// Revival Input Test
    //FTimerHandle testRevival;
    //auto lambda3 = [&]()
    //    {
    //        UE_LOG(LogTemp, Warning, TEXT("[REVIVAL TEST] DEAD!!! Current HP : %d"), 0);
    //        OnDamaged(12);
    //    };
    //GetWorld()->GetTimerManager().SetTimer(testRevival, lambda3, 45.f, false);
}

void ACPlayer::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Dash
    if (bCanDash) DoDash(DeltaTime);
    if (bCanResetDash) ResetDash(DeltaTime);

    // Aim
    AdjustTargetArmLocation(DeltaTime);

    // Revive
    if(bIsReviving) OnRevive(DeltaTime);

    // God Mode
    if(bIsGodMode) GodMode(DeltaTime);
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

void ACPlayer::OnDamaged(int32 InDamage)
{
    // ���� ������ ���� ������ ó�� X
    if (bIsGodMode)
    {
        UE_LOG(LogTemp, Error, TEXT("###### GOOOOOOOOD MOOOOOOOD ######"));
        return;
    }

    if(bIsDead) return;

    UE_LOG(LogTemp, Error, TEXT("###### PLAYER GET DAMAGE ######"));

    bIsDamaged = true;
    HP -= InDamage;
    if(HP <= 0)
    {
        OnDead();
        return;
    }

    //RecoverHP();
    // ���� �ð��� ������ ȸ���Ѵ�
    //FTimerHandle damageTimer;
    auto lambda = [&]() {
        UE_LOG(LogTemp, Warning, TEXT(">>>>>> RECOVER START"));
            GetWorld()->GetTimerManager().ClearTimer(DamageTimer);
            RecoverHP();
        };
    GetWorld()->GetTimerManager().SetTimer(DamageTimer, lambda, DamageDurationTime, false);
}

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

void ACPlayer::RecoverHP()
{
    //FTimerHandle recoverTimer;
    auto lambda = [&](){
        if(HP < MaxHP)
        {
            HP++;
            UE_LOG(LogTemp, Warning, TEXT("[Recover] Current HP : %d"), HP);
        }
        else
        {
            bIsDamaged = false;
            GetWorld()->GetTimerManager().ClearTimer(RecoverTimer);
        }
            };
    GetWorld()->GetTimerManager().SetTimer(RecoverTimer, lambda, RecoverTime, true);
}

void ACPlayer::OnDead()
{
    bIsDead = true;

    // Skeletal Mesh Visibility ����
    GetMesh()->SetVisibility(false);
    // GunMesh�� Visibility ����
    Gun->GetGunMeshComp()->SetVisibility(false);
    //Actor Location ��ġ ���Ϸ� ����
    SetActorLocation(GetActorLocation() + -GetActorUpVector() * 100);
    // ���� FX�� Visibility�� �Ҵ�

    // ���� FX�� ������
    // ȭ�� ä���� ��¦ ��������
    // ȭ���� �Ѿ�����
    // ��Ȱ Ÿ�̸Ӱ� ���۵ȴ�
    bIsReviving = true;
    // ��Ȱ UI�� ���
    // E ��ư�� ������ ��Ȱ�ϴµ� �ɸ��� �ð��� �پ���
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
            UE_LOG(LogTemp, Warning, TEXT(">>>>> Revival Input Boost <<<"));
            CurrentReviveTime += InDeltaTime * ReviveBoostAmount;
            bIsRevivalInputEntered = false;
        }
        else CurrentReviveTime += InDeltaTime;

        DebugReviveTime += InDeltaTime;
    }
    else
    {
        //Skeletal Mesh Visibility ����
        GetMesh()->SetVisibility(true);
        // GunMesh�� Visibility ����
        Gun->GetGunMeshComp()->SetVisibility(true);
        //���� FX Visible �ѱ�
        //������ ������
        SetActorLocation(FVector(0));
        SetActorRotation(FRotator(0, 180, 0));
        // ���� �� ��Ȱ ���� ������ �ʱ�ȭ
        bIsReviving = false;
        bIsDead = false;
        bIsDamaged = false;
        HP = MaxHP;
        //���� �ð����� Enemy�� �浹�ص� ���� ���°� �ǰ� Collision �������ְ�
        //���� �ð� ������ ���� Collision ���·� ���ƿ�����
        UE_LOG(LogTemp, Warning, TEXT("----- REVIVAl COMPLETE : %f / %f (Time Spent / Time Max) -----"), DebugReviveTime, RevivalTime);
        CurrentReviveTime = 0;
        DebugReviveTime = 0;
        bIsGodMode = true;
    }
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

    // ���콺 ����
    float mouseSensitivity = (bCanAim) ? MouseSensitivityAim : MouseSensitivityDefault;

    // pitch : ȸ�� ��������
    // ��Ȱ ���� �� ���ݸ� ȸ���ϰ�
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

    UE_LOG(LogTemp, Warning, TEXT("[Change Speed] Current Speed : %f"), GetCharacterMovement()->MaxWalkSpeed);
}

void ACPlayer::StartDash(const FInputActionValue& InValue)
{
    if (bIsDead || bIsReviving) return;

    // �̹� Dash ���̰ų� Dash cool down ���̶�� �ƹ� ó������ �ʴ´�
    if (bCanDash || bCanResetDash) return;

    DashStartPos = GetActorLocation();
    DashEndPos = GetActorLocation() + GetActorForwardVector() * DashDistance;

    // Dash�� ���۵Ǿ����� ����Ѵ�
    bCanDash = true;
}

void ACPlayer::DoDash(float InDeltaTime)
{
    DashCurrentTime += InDeltaTime;
    float ratio = DashCurrentTime / DashDurationTime;

    SetActorLocation( FMath::Lerp(DashStartPos, DashEndPos, ratio) );

    if (DashCurrentTime >= DashDurationTime)
    {
        // ��ġ ����
        SetActorLocation(DashEndPos);

        UE_LOG(LogTemp, Log, TEXT(">>> Dash Complete"));

        // Dash�� �������� ���
        bCanDash = false;
        DashCurrentTime = 0;

        // Dash CoolDown�� ���۵Ǿ����� ���
        bCanResetDash = true;
        UE_LOG(LogTemp, Log, TEXT(">>> Dash Cool Down Start"));
    }
}

void ACPlayer::ResetDash(float InDeltaTime)
{
    DashCurrentTime += InDeltaTime;

    if (DashCurrentTime >= DashCoolDownTime)
    {
        // Cool down�� �������� ���
        bCanResetDash = false;
        DashCurrentTime = 0;

        UE_LOG(LogTemp, Log, TEXT(">>> Dash Cool Down Complete"));
    }
}

void ACPlayer::InitCrosshairWidgets()
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

void ACPlayer::SetUnlockedCrosshairVisibility(bool bVisible)
{
    if (UnlockedCrossshairUI)
        UnlockedCrossshairUI->SetVisibility( bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden );
}

void ACPlayer::SetLockedCrosshairVisibility(bool bVisible)
{
    if (LockedCrossshairUI)
        LockedCrossshairUI->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
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
    if (bIsDead || bIsDamaged || bIsReviving) return;

    bCanAim = true;

    if (CameraBoom->GetComponentLocation() == CameraBoomLocationDefault) ZoomCurrentTime = 0;

    // UnlockedCrosshairUI�� Visible ���ֱ�
    if (UnlockedCrossshairUI) SetUnlockedCrosshairVisibility(true);

    // Player�� Move �������� ȸ���ϵ��� ����
    bUseControllerRotationYaw = true;

    // CameraBoom Location ����
    bCanZoom = true;
}

void ACPlayer::AdjustTargetArmLocation(float InDeltaTime)
{
    //UE_LOG(LogTemp, Warning, TEXT(">>> Start Adjust Target Arm Location <<<"));

    //ZoomCurrentTime += InDeltaTime;
    float ratio;

    if (bCanZoom) ZoomCurrentTime += InDeltaTime;
    else ZoomCurrentTime -= InDeltaTime;

    // CurTime�� ������ ����� ���� ��������
    ZoomCurrentTime = FMath::Clamp(ZoomCurrentTime, 0, AimZoomMaxTime);

    ratio = EaseInOutQuad(ZoomCurrentTime / AimZoomMaxTime);
    CameraBoom->SetRelativeLocation(FMath::Lerp(CameraBoomLocationDefault, CameraBoomLocationZoomIn, ratio));
}

void ACPlayer::TriggerAim(const FInputActionValue& InValue)
{
    if (bIsDead || bIsReviving) return;

    FVector startPos = Gun->GetFirePosition();
    FVector endPos = startPos + PlayerCamear->GetForwardVector() * SphereTraceDistance;

    FHitResult hitResult;
    TArray<AActor*> actorsToIgnore;
    actorsToIgnore.Add(this);

    bool bHit = UKismetSystemLibrary::SphereTraceSingle(this, startPos, endPos, SphereTraceRadius, UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel6), false, actorsToIgnore, EDrawDebugTrace::ForDuration, hitResult, true, FColor::Purple, FColor::Orange, 0.3f);
    
    if (bHit)
    {
        FireDestination = hitResult.Location;

        bool bHitBySap = hitResult.GetComponent()->ComponentHasTag(FName("Sap"));
        //bool bHitBySapCenter = hitResult.GetComponent()->ComponentHasTag(FName("SapCenter"));

        //UE_LOG(LogTemp, Warning, TEXT("[HIT] bHitBySap : %d / bHitBySapCenter : %d"), bHitBySap, bHitBySapCenter);

        // Sap�� ����ִ� �뿡 ��Ҵٸ�
        if (bHitBySap)
        {
            // UnlockedCrosshair�� ���ش�
            SetUnlockedCrosshairVisibility(false);

            // LockedCrossHair�� ���ش�
            SetLockedCrosshairVisibility(true);

            // Sap Center�� ��ġ�� �����´�
            //FVector sapCenterLocation;
            //TArray<UActorComponent*> components = hitResult.GetActor()->GetComponentsByTag(USphereComponent::StaticClass(), FName("SapCenter"));
            //for (UActorComponent* c : components)
            //{
            //    USphereComponent* sapCenter = Cast<USphereComponent>(c);
            //    if(sapCenter) sapCenterLocation = sapCenter->GetComponentLocation() + sapCenter->GetUpVector() * 10;
            //    // 10 : SapCenter�� ������ ����
            //}

            FVector sapCenterLocation;
            USphereComponent* sapFull = Cast<USphereComponent>(hitResult.GetComponent());
            if (sapFull)
            {
                sapCenterLocation = sapFull->GetComponentLocation() + sapFull->GetUpVector() * sapFull->GetScaledSphereRadius();
                FireDestination = sapCenterLocation;
                // Sap Center�� ��ũ�� ��ǥ ��ġ�� ���Ѵ�
                FVector2D screenPos;
                APlayerController* pc = Cast<APlayerController>(GetController());
                // ������ true : DPI Scaling�� �����ϰڴ�
                bool bProjected = UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(pc, sapCenterLocation, screenPos, false);

                if (bProjected)
                {
                    // LockedCrossHair�� ��ġ�� FireDestination�� ��ũ�� ��ǥ ��ġ�� �������ش�
                    LockedCrossshairUI->UpdateCrosshairPosition(screenPos);
                    //UE_LOG(LogTemp, Warning, TEXT("---screenPos : %s"), *screenPos.ToString());
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


void ACPlayer::CompleteAim(const FInputActionValue& InValue)
{
    // UI�� Visible ���ֱ�
    SetUnlockedCrosshairVisibility(false);
    SetLockedCrosshairVisibility(false);

    // ���콺 Ŀ�� �������� ȸ���ϵ��� ����
    bUseControllerRotationYaw = false;

    // CameraBoom Location ����
    bCanZoom = false;

    bCanAim = false;
}


void ACPlayer::AttachGun()
{
    if (GunBP)
    {
        Gun = GetWorld()->SpawnActor<ACGun>(GunBP);
        if(Gun) Gun->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("Gun"));
        UE_LOG(LogTemp, Error, TEXT(">>> Attach Gun Success"));
    }
}

void ACPlayer::DoFire()
{
    if (bIsDead || bIsReviving) return;

    // Aim ��尡 �ƴϰų�, �Ѿ��� ���� �����Ǿ��ų�, Fire ���̶��
    // MAY
    if(!bCanAim || CurrentBulletCount == 0 || bIsInFireDelayTime) return;
    //// CODY
    //if (!bCanAim || CurrentSapAmout <= 0 || bIsInFireDelayTime) return;

    UE_LOG(LogTemp, Error, TEXT(">>>>> Fire Input Entered <<<<<"));
    // MAY
    CurrentBulletCount--;
    Gun->FireBullet(FireDestination);
    bIsInFireDelayTime = true;
    UE_LOG(LogTemp, Error, TEXT(">>> Current Bullet : %d"), CurrentBulletCount);

    // Fire Delay Time ���ȿ��� Fire �Ұ�
    FTimerHandle fireDelayTimer;
    auto fireDelayLambda = [&]() { bIsInFireDelayTime = false; };
    GetWorld()->GetTimerManager().SetTimer(fireDelayTimer, fireDelayLambda, FireDelayTime, false);

    // MAY
    // ���� �ð��� ������ Ammo �ڵ� ����
    FTimerHandle chargeAmmoTimer;
    auto chargeAmmoLambda = [&]() {
            if(CurrentBulletCount >= MaxBulletCount) return;
            CurrentBulletCount++;
            UE_LOG(LogTemp, Error, TEXT(">>> Current Bullet : %d"), CurrentBulletCount);
        };
    GetWorld()->GetTimerManager().SetTimer(chargeAmmoTimer, chargeAmmoLambda, ChargeAmmoTime, false);
}
