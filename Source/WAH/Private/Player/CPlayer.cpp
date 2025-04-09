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
    CameraBoom->SetRelativeLocation(FVector(0, 0, 20));

    /* Camera */
    PlayerCamear = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
    PlayerCamear->SetupAttachment(CameraBoom/*, USpringArmComponent::SocketName*/);
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

    if (bCanDash) DoDash(DeltaTime);
    if (bCanResetDash) ResetDash(DeltaTime);
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
        inputSystem->BindAction(IA_Aim, ETriggerEvent::Triggered, this, &ACPlayer::DoAim);
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

    // ȸ�� ����
    float pitch = FMath::Clamp(GetController()->GetControlRotation().Pitch + -(scale.Y), MinPitch, MaxPitch);
    float yaw = GetController()->GetControlRotation().Yaw + scale.X;

    GetController()->SetControlRotation(FRotator(pitch, yaw, GetController()->GetControlRotation().Roll));
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
    FMath::Lerp(InStartVal, InEndVal, InRatio);
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

    FVector currentPos = Zoom(DashStartPos, DashEndPos, ratio);
    SetActorLocation(currentPos);

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

void ACPlayer::DoAim(const FInputActionValue& InValue)
{
    // UnlockedCrosshairUI�� Visible ���ֱ�
    if(UnlockedCrossshairUI) SetUnlockedCrosshairVisibility(true);

    // Camera Boom�� Arm Length ����
}


void ACPlayer::CompleteAim()
{
    // UI�� Visible ���ֱ�
    SetUnlockedCrosshairVisibility(false);
    SetLockedCrosshairVisibility(false);

    // Camera Boom�� Arm Length ����
}

