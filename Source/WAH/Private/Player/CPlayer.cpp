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
#include "../../../../../../../Source/Runtime/Engine/Classes/Components/SceneCaptureComponent2D.h"
#include "UI/CBattleUI.h"
#include "Sys/WPlayerController.h"

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

    SceneCapture2D = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCapture2D"));
    SceneCapture2D->SetupAttachment(CameraBoom);

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

    if (IsLocallyControlled() && HasAuthority() == false)
    {
        InItBattleWidget();
    }
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

        // Jump
        ServerRPC_UpdateJumpInfo();
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

void ACPlayer::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    if (IsLocallyControlled())
    {
        InItBattleWidget();
    }
}

void ACPlayer::OnRep_HP()
{
    GEngine->AddOnScreenDebugMessage(0, 2, FColor::Red, FString::Printf(TEXT("[DAMAGED] Player Get DAMAGED : %d"), HP));

    auto pc = Cast<AWPlayerController>(Controller);
    if(pc) BattleUI->UpdateMPCPlayerHP(bIsCody, HP, MaxHP);

    bIsDamaged = true;
    if (HP <= 0)
    {
        OnDead();
        return;
    }
}

void ACPlayer::OnDamaged(int32 InDamage)
{
    //if (bIsGodMode || bIsDead) return;

    if (bIsGodMode) 
    {
        UE_LOG(LogTemp, Warning, TEXT("[GOD MODE] Damage Ignore"));
        return;
    }

    if (bIsDead)
    {
        UE_LOG(LogTemp, Warning, TEXT("[DEAD MODE] Damage Ignore"));
        return;
    }

    HP -= InDamage;
    OnRep_HP();

    auto lambda = [&]() {
        GetWorld()->GetTimerManager().ClearTimer(DamageTimer);

        auto lambda = [&]() {
            GEngine->AddOnScreenDebugMessage(0, 2, FColor::Red, TEXT("[DAMAGED] TIMER END @@@@@@@@@@@@@@"));
            if (HP < MaxHP)
            {
                GEngine->AddOnScreenDebugMessage(0, 2, FColor::Red, FString::Printf(TEXT("[DAMAGED] Current HP : %d"), HP));
                HP++;
                OnRep_HP();
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

void ACPlayer::OnDead()
{
    MulticastRPC_Dead();
}

void ACPlayer::RevivalInputEntered(const FInputActionValue& InValue)
{
    ServerRPC_RevivalInputEntered();
}

void ACPlayer::ServerRPC_RevivalInputEntered_Implementation()
{
    if (bIsReviving) bIsRevivalInputEntered = true;
}

void ACPlayer::OnRevive(float InDeltaTime)
{
    if (CurrentReviveTime < RevivalTime)
    {
        if (bIsRevivalInputEntered)
        {
            ServerRPC_ReviveInputEntered(InDeltaTime);
        }
        else CurrentReviveTime += InDeltaTime;
        DebugReviveTime += InDeltaTime;
    }
    else
    {
        MulticastRPC_Revive();
    }
}

void ACPlayer::ServerRPC_ReviveInputEntered_Implementation(float InDeltaTime)
{
    MulticastRPC_ReviveInputEntered(InDeltaTime);
}

void ACPlayer::MulticastRPC_ReviveInputEntered_Implementation(float InDeltaTime)
{
    GEngine->AddOnScreenDebugMessage(2, 0.2, FColor::Green, TEXT("[REVIVAL] E Entered"));
    CurrentReviveTime += InDeltaTime * ReviveBoostAmount;
    bIsRevivalInputEntered = false;
}

void ACPlayer::MulticastRPC_Dead_Implementation()
{
    bIsDead = true;

    GEngine->AddOnScreenDebugMessage(0, 2, FColor::Red, TEXT("[DEAD] Player DEAD!!!!!"));
    GetMesh()->SetVisibility(false);
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
    GEngine->AddOnScreenDebugMessage(1, 2, FColor::Red, FString::Printf(TEXT("[REVIVAL] Player REVIVE Complete - %f / %f"), DebugReviveTime, RevivalTime));

    GetMesh()->SetVisibility(true);

    //등장 FX Visible 켜기

    SetActorLocationAndRotation(SpawnPoint, SpawnRotation);

    bIsReviving = false;
    bIsDead = false;
    bIsDamaged = false;
    HP = MaxHP;
    OnRep_HP();

    CurrentReviveTime = 0;
    DebugReviveTime = 0;
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

    //Jump();
    ServerRPC_DoJump();
}

void ACPlayer::ServerRPC_UpdateJumpInfo_Implementation()
{
    //UE_LOG(LogTemp, Warning, TEXT("Server >> JUMP"));
    MulticastRPC_UpdateJumpInfo();
}

void ACPlayer::MulticastRPC_UpdateJumpInfo_Implementation()
{
    bIsFalling = GetCharacterMovement()->IsFalling();
    PlayerJumpCurrentCount = JumpCurrentCount;
}

void ACPlayer::ServerRPC_DoJump_Implementation()
{
    MulticastRPC_DoJump();
}

void ACPlayer::MulticastRPC_DoJump_Implementation()
{
    Jump();
}

void ACPlayer::DoRun(const FInputActionValue& InValue)
{
    if (bCanAim || bIsDead || bIsReviving) return;

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
}

void ACPlayer::ServerRPC_StartDash_Implementation()
{
    DashStartPos = GetActorLocation();
    DashEndPos = GetActorLocation() + GetActorForwardVector() * DashDistance;

    MulticastRPC_UpdateCanDash(true);
}

void ACPlayer::MulticastRPC_UpdateCanDash_Implementation(bool InResult)
{
    bCanDash = InResult;
}

void ACPlayer::DoDash(float InDeltaTime)
{
    DashCurrentTime += InDeltaTime;
    float ratio = DashCurrentTime / DashDurationTime;

    SetActorLocation( FMath::Lerp(DashStartPos, DashEndPos, ratio) );

    if (DashCurrentTime >= DashDurationTime)
    {
        SetActorLocation(DashEndPos);
        MulticastRPC_UpdateCanDash(false);
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
}

void ACPlayer::ServerRPC_UpdateUIHP_Implementation()
{
    MulticastRPC_UpdateUIHP();
}

void ACPlayer::MulticastRPC_UpdateUIHP_Implementation()
{
    UE_LOG(LogTemp, Warning, TEXT("bIsCody : %d"), bIsCody);

    BattleUI->UpdateMPCPlayerHP(bIsCody, HP, MaxHP);
}

void ACPlayer::InItBattleWidget()
{
    if (BattleWidget && BattleUI == nullptr)
    {
        BattleUI = Cast<UCBattleUI>(CreateWidget(GetWorld(), BattleWidget));
        BattleUI->SetVisibility(ESlateVisibility::Visible);
        BattleUI->AddToViewport();
    }
}

void ACPlayer::SetUnlockedCrosshairVisibility(bool bVisible)
{
}

void ACPlayer::SetLockedCrosshairVisibility(bool bVisible)
{
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
}

void ACPlayer::AdjustTargetArmLocation(float InDeltaTime)
{
}

void ACPlayer::TriggerAim(const FInputActionValue& InValue)
{
}


void ACPlayer::CompleteAim(const FInputActionValue& InValue)
{
}


void ACPlayer::AttachGun()
{
}

void ACPlayer::DoFire()
{
}

void ACPlayer::PrintNetLog()
{
    const FString ownerName = GetOwner() != nullptr ? GetOwner()->GetName() : TEXT("No Owner");
    float speed = GetCharacterMovement()->Velocity.Size();
    FString myLog = FString::Printf(TEXT("%s Speed : %f"), *ownerName, speed);

    DrawDebugString(GetWorld(), GetActorLocation() + FVector::UpVector * 100.0f, myLog, nullptr, FColor::White, 0, true);
}

void ACPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    //DOREPLIFETIME(ACPlayer, );
    DOREPLIFETIME(ACPlayer, bIsCody);
    DOREPLIFETIME(ACPlayer, HP);
    DOREPLIFETIME(ACPlayer, bCanDash);
    DOREPLIFETIME(ACPlayer, bCanResetDash);
    DOREPLIFETIME(ACPlayer, FireDestination);
    DOREPLIFETIME(ACPlayer, bCanAim);
    DOREPLIFETIME(ACPlayer, bCanZoom);

    DOREPLIFETIME(ACPlayer, bIsDamaged);
    DOREPLIFETIME(ACPlayer, bIsDead);
    DOREPLIFETIME(ACPlayer, bIsReviving);
    DOREPLIFETIME(ACPlayer, bIsGodMode);

    DOREPLIFETIME(ACPlayer, bIsRevivalInputEntered);
    DOREPLIFETIME(ACPlayer, CurrentReviveTime);
    DOREPLIFETIME(ACPlayer, DebugReviveTime);

    DOREPLIFETIME(ACPlayer, bIsFalling);
    DOREPLIFETIME(ACPlayer, PlayerJumpCurrentCount);
}

#pragma region TEST
void ACPlayer::TestDamage(const FInputActionValue& InValue)
{
    UE_LOG(LogTemp, Warning, TEXT("[DAMAGE TEST] DAMAGED!!! Current HP : %d"), HP);
    OnDamaged(6);
}

void ACPlayer::TestRevival(const FInputActionValue& InValue)
{
    UE_LOG(LogTemp, Warning, TEXT("[REVIVAL TEST] DEAD!!! Current HP : %d"), HP);
    OnDamaged(12);
}
#pragma endregion