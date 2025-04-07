#include "Player/CPlayer.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

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
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>( TEXT("CameraBoom") );
	CameraBoom->SetupAttachment( RootComponent );
	CameraBoom->TargetArmLength = 400.f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SetRelativeLocation( FVector(0, 0, 20) );

	/* Camera */
	PlayerCamear = CreateDefaultSubobject<UCameraComponent>( TEXT("PlayerCamera") );
	PlayerCamear->SetupAttachment(CameraBoom/*, USpringArmComponent::SocketName*/);
	PlayerCamear->bUsePawnControlRotation = false;

	/* IMC */
	ConstructorHelpers::FObjectFinder<UInputMappingContext> tmpIMC ( TEXT("/Script/EnhancedInput.InputMappingContext'/Game/DYL/Inputs/IMC_Player.IMC_Player'"));
	if( tmpIMC.Succeeded() ) IMC_Player = tmpIMC.Object;

	/* IA */
	ConstructorHelpers::FObjectFinder<UInputAction> tmpIAMove ( TEXT("/Script/EnhancedInput.InputAction'/Game/DYL/Inputs/IA_Move.IA_Move'") );
	if( tmpIAMove.Succeeded() ) IA_Move = tmpIAMove.Object;

	ConstructorHelpers::FObjectFinder<UInputAction> tmpIATurn ( TEXT("/Script/EnhancedInput.InputAction'/Game/DYL/Inputs/IA_Turn.IA_Turn'") );
	if( tmpIATurn.Succeeded() ) IA_Turn = tmpIATurn.Object;

	ConstructorHelpers::FObjectFinder<UInputAction> tmpIAJump(TEXT("/Script/EnhancedInput.InputAction'/Game/DYL/Inputs/IA_Jump.IA_Jump'"));
	if (tmpIATurn.Succeeded()) IA_Jump = tmpIAJump.Object;

	ConstructorHelpers::FObjectFinder<UInputAction> tmpIARun(TEXT("/Script/EnhancedInput.InputAction'/Game/DYL/Inputs/IA_Run.IA_Run'"));
	if (tmpIARun.Succeeded()) IA_Run = tmpIARun.Object;

	ConstructorHelpers::FObjectFinder<UInputAction> tmpIADash(TEXT("/Script/EnhancedInput.InputAction'/Game/DYL/Inputs/IA_Dash.IA_Dash'"));
	if (tmpIADash.Succeeded()) IA_Dash = tmpIADash.Object;
}

void ACPlayer::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// IMC
	APlayerController* pc = Cast<APlayerController>( GetController() );
	if (pc)
	{
		auto localPlayer = pc->GetLocalPlayer();
		auto ss = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>( localPlayer );
		if(ss) ss->AddMappingContext( IMC_Player, 0);
	}
	
	// IA
	auto inputSystem = Cast<UEnhancedInputComponent>( PlayerInputComponent );
	if (inputSystem)
	{
		inputSystem->BindAction(IA_Move, ETriggerEvent::Triggered, this, &ACPlayer::DoMove);
		inputSystem->BindAction(IA_Turn, ETriggerEvent::Triggered, this, &ACPlayer::DoTurn);
		inputSystem->BindAction(IA_Jump, ETriggerEvent::Started, this, &ACPlayer::DoJump);
		inputSystem->BindAction(IA_Run, ETriggerEvent::Started, this, &ACPlayer::DoRun);
		inputSystem->BindAction(IA_Dash, ETriggerEvent::Started, this, &ACPlayer::StartDash);
	}
}

void ACPlayer::DoMove(const struct FInputActionValue& InValue)
{
	if( bCanDash ) return;

	FVector2D scale = InValue.Get<FVector2D>();

	AddMovementInput(PlayerCamear->GetForwardVector() * scale.X + PlayerCamear->GetRightVector() * scale.Y);
}

void ACPlayer::DoTurn(const FInputActionValue& InValue)
{
	FVector2d scale = InValue.Get<FVector2d>();

	// ȸ�� ����
	float pitch = FMath::Clamp(GetActorRotation().Pitch + scale.Y, -30.f, 90.f);
	//float yaw = FMath::Clamp(GetActorRotation().Yaw + scale.X, -45.f, 45.f);

	//GetController()->SetControlRotation( FRotator(scale.Y, scale.X, GetController()->GetControlRotation().Roll) );
	AddControllerPitchInput( pitch );
	AddControllerYawInput( scale.X );
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

void ACPlayer::StartDash(const FInputActionValue& InValue)
{
	// �̹� Dash ���̰ų� Dash cool down ���̶�� �ƹ� ó������ �ʴ´�
	if (bCanDash || bIsDashCoolDown) return;

	DashDestination = GetActorLocation() + GetActorForwardVector() * DashDistance;

	// Dash�� ���۵Ǿ����� ����Ѵ�
	bCanDash = true;
	DoDash();
}

void ACPlayer::DoDash()
{
	UE_LOG(LogTemp, Warning, TEXT(">>>> Dash"));

	// Dash Lambda 
	auto lambda = [this]()
		{
			FVector currentPos = GetActorLocation();
			FVector targetPos = DashDestination;

			// ���� ��ġ���� DashDistance��ŭ �̵�
			currentPos = FMath::Lerp(currentPos, targetPos, 10 * GetWorld()->DeltaTimeSeconds);
			SetActorLocation(currentPos);
			//UE_LOG(LogTemp, Log, TEXT(">>> Dash ing - distance : %f"), FVector::Distance(currentPos, targetPos));

			// �������κ��� ���� ���� �ȿ� �����ߴٸ�
			if (FVector::Distance(currentPos, targetPos) < 10)
			{
				// ��ġ ����
				SetActorLocation(targetPos);
				CompleteDash();
			}
		};

	// DashDurationTime ���� Dash�� �̵�
	GetWorldTimerManager().SetTimer(DashTimer, lambda, DashDurationTime, true);
}



void ACPlayer::CompleteDash()
{
	UE_LOG(LogTemp, Log, TEXT(">>> Dash Complete"));
	// Dash Ÿ�̸� ����
	GetWorldTimerManager().ClearTimer(DashTimer);

	// Dash�� �������� ���
	bCanDash = false;

	// Dash CoolDown�� ���۵Ǿ����� ���
	bIsDashCoolDown = true;
	UE_LOG(LogTemp, Log, TEXT(">>> Dash Cool Down Start"));

	// Dash Cooldown ����
	auto lambda = [this]()
		{
			// Cool down�� �������� ���
			bIsDashCoolDown = false;

			// Dash Cool Down Timer �ʱ�ȭ
			GetWorldTimerManager().ClearTimer(DashCoolDownTimer);

			UE_LOG(LogTemp, Log, TEXT(">>> Dash Cool Down Complete"));
		};

	// DashCoolDownTime ���� Cool Down ����
	GetWorldTimerManager().SetTimer(DashCoolDownTimer, lambda, DashDurationTime, false);
}

