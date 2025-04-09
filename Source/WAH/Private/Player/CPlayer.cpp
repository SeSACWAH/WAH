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
}

void ACPlayer::BeginPlay()
{
    Super::BeginPlay();

    // Crosshair
    InitCrosshairWidgets();
}

void ACPlayer::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Dash
    if (bCanDash) DoDash(DeltaTime);
    if (bCanResetDash) ResetDash(DeltaTime);

    // Aim
    //if (bCanZoomIn || bCanZoomOut) AdjustTargetArmLength(DeltaTime);
    if (bCanZoomIn || bCanZoomOut) AdjustTargetArmLocation(DeltaTime);
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
        inputSystem->BindAction(IA_Aim, ETriggerEvent::Completed, this, &ACPlayer::CompleteAim);
        
    }
}

void ACPlayer::DoMove(const struct FInputActionValue& InValue)
{
    if (bCanDash) return;

    FVector2D scale = InValue.Get<FVector2D>();

    AddMovementInput(PlayerCamear->GetForwardVector() * scale.X + PlayerCamear->GetRightVector() * scale.Y);
}

void ACPlayer::DoTurn(const FInputActionValue& InValue)
{
    FVector2d scale = InValue.Get<FVector2d>();

    // ���콺 ����
    float mouseSensitivity = (bCanAim) ? MouseSensitivityAim : MouseSensitivityDefault;

    // pitch : ȸ�� ��������
    float pitch = FMath::Clamp(GetController()->GetControlRotation().Pitch + scale.Y * mouseSensitivity, MinPitch, MaxPitch);
    float yaw = GetController()->GetControlRotation().Yaw + scale.X * mouseSensitivity;
    float roll = GetController()->GetControlRotation().Roll* mouseSensitivity;

    GetController()->SetControlRotation(FRotator(pitch, yaw, roll));
}

void ACPlayer::DoJump(const FInputActionValue& InValue)
{
    Jump();
}

void ACPlayer::DoRun(const FInputActionValue& InValue)
{
    if (GetCharacterMovement()->MaxWalkSpeed > SpeedJog)
        GetCharacterMovement()->MaxWalkSpeed = SpeedJog;
    else
        GetCharacterMovement()->MaxWalkSpeed = SpeedRun;
}

template <typename T>
T ACPlayer::Zoom(T InStartVal, T InEndVal, float InRatio)
{
    return FMath::Lerp(InStartVal, InEndVal, InRatio);
}

void ACPlayer::StartDash(const FInputActionValue& InValue)
{
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

// for Zoom In
float ACPlayer::EaseOutExpo(float InRatio)
{
    return InRatio == 1 ? 1 : 1 - FMath::Pow(2, -10 * InRatio);
}

// for Zoom Out
float ACPlayer::EaseOutSine(float InRatio)
{
    return FMath::Sin((InRatio * PI) / 2);
}

void ACPlayer::StartAim(const FInputActionValue& InValue)
{
    bCanAim = true;

    // UnlockedCrosshairUI�� Visible ���ֱ�
    if (UnlockedCrossshairUI) SetUnlockedCrosshairVisibility(true);

    // Player�� Move �������� ȸ���ϵ��� ����
    bUseControllerRotationYaw = true;

    // CameraBoom Location ����
    ZoomCurrentTime = 0;
    bCanZoomIn = true;
}

void ACPlayer::AdjustTargetArmLength(float InDeltaTime)
{
    UE_LOG(LogTemp, Warning, TEXT(">>> Start Adjust Target Arm Length <<<"));
    
    ZoomCurrentTime += InDeltaTime;
    float durTime, ratio;

    if (bCanZoomIn)
    {
        durTime = AimZoomInTime;
        ratio = EaseOutExpo(ZoomCurrentTime / AimZoomInTime);
        CameraBoom->TargetArmLength = FMath::Lerp(ArmLengthDefault, ArmLengthAim, ratio);
        UE_LOG(LogTemp, Error, TEXT(">>> Zoom INNNNNNNNNNN <<<"));
    }

    if(bCanZoomOut)
    {
        durTime = AimZoomOutTime;
        ratio = EaseOutSine(ZoomCurrentTime / AimZoomOutTime);
        CameraBoom->TargetArmLength = FMath::Lerp(ArmLengthAim, ArmLengthDefault, ratio);
        UE_LOG(LogTemp, Error, TEXT(">>> Zoom OUTTTTTTTTT <<<"));
    }

    // UE_LOG(LogTemp, Error, TEXT(">>> Cur Time : %f / Dur Time : %f / Target Arm Length : %f"), AimCurrentTime, durTime, CameraBoom->TargetArmLength);

    if (ZoomCurrentTime >= durTime)
    {
        // ��ġ ����
        //CameraBoom->TargetArmLength = destLen;

        ZoomCurrentTime = 0;
        bCanZoomIn = false;
        bCanZoomOut = false;

        UE_LOG(LogTemp, Warning, TEXT(">>> Complete Adjust Target Arm Length <<<"));
    }
}

void ACPlayer::AdjustTargetArmLocation(float InDeltaTime)
{
    UE_LOG(LogTemp, Warning, TEXT(">>> Start Adjust Target Arm Location <<<"));

    ZoomCurrentTime += InDeltaTime;
    float durTime, ratio;
    FVector destLoc;

    if (bCanZoomIn)
    {
        durTime = AimZoomInTime;
        ratio = EaseOutExpo(ZoomCurrentTime / AimZoomInTime);
        destLoc = CameraBoomLocationZoomIn;
        CameraBoom->SetRelativeLocation( FMath::Lerp(CameraBoomLocationDefault, CameraBoomLocationZoomIn, ratio) );
        UE_LOG(LogTemp, Error, TEXT(">>> Zoom INNNNNNNNNNN <<<"));
    }

    if (bCanZoomOut)
    {
        durTime = AimZoomOutTime;
        ratio = EaseOutSine(ZoomCurrentTime / AimZoomOutTime);
        destLoc = CameraBoomLocationDefault;
        CameraBoom->SetRelativeLocation(FMath::Lerp(CameraBoomLocationZoomIn, CameraBoomLocationDefault, ratio));
        UE_LOG(LogTemp, Error, TEXT(">>> Zoom OUTTTTTTTTT <<<"));
    }

    // ���� �ð� �ȿ� ���԰ų� ���� �Ÿ� �ȿ� ���Դٸ�
    if (ZoomCurrentTime >= durTime || FVector::Distance(CameraBoom->GetRelativeLocation(), destLoc) < 2)
    {
        // ��ġ ����
        CameraBoom->SetRelativeLocation(destLoc);

        ZoomCurrentTime = 0;
        bCanZoomIn = false;
        bCanZoomOut = false;

        UE_LOG(LogTemp, Warning, TEXT(">>> Complete Adjust Target Arm Location <<<"));
    }
}


void ACPlayer::CompleteAim()
{
    // UI�� Visible ���ֱ�
    SetUnlockedCrosshairVisibility(false);
    SetLockedCrosshairVisibility(false);

    // ���콺 Ŀ�� �������� ȸ���ϵ��� ����
    bUseControllerRotationYaw = false;

    // CameraBoom Location ����
    bCanZoomOut = true;

    bCanAim = false;
}

