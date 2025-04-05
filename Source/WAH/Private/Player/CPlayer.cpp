#include "Player/CPlayer.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ACPlayer::ACPlayer()
{
	PrimaryActorTick.bCanEverTick = true;

	/* Controller Rotation */
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	/* Character Movement */
	GetCharacterMovement()->bOrientRotationToMovement = false; 
	//GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->MaxWalkSpeed = SpeedJog;

	/* Jump */
	JumpMaxCount = 2;

	/* Camera Boom */
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>( TEXT("CameraBoom") );
	CameraBoom->SetupAttachment( RootComponent );
	CameraBoom->TargetArmLength = 200.f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SetRelativeLocation( FVector(0, 50, 70) );
	CameraBoom->SetRelativeRotation( FRotator(-10, 0, 0) );

	/* Camera */
	PlayerCamear = CreateDefaultSubobject<UCameraComponent>( TEXT("PlayerCamera") );
	PlayerCamear->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
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
		inputSystem->BindAction(IA_Move, ETriggerEvent::Triggered, this, &ACPlayer::PlayerMove);
		inputSystem->BindAction(IA_Turn, ETriggerEvent::Triggered, this, &ACPlayer::PlayerTurn);
		inputSystem->BindAction(IA_Jump, ETriggerEvent::Started, this, &ACPlayer::PlayerJump);
		inputSystem->BindAction(IA_Run, ETriggerEvent::Started, this, &ACPlayer::PlayerRun);
		inputSystem->BindAction(IA_Dash, ETriggerEvent::Started, this, &ACPlayer::PlayerDash);
	}
}

void ACPlayer::PlayerMove(const struct FInputActionValue& InValue)
{
	FVector2D scale = InValue.Get<FVector2D>();

	AddMovementInput(PlayerCamear->GetForwardVector() * scale.X + PlayerCamear->GetRightVector() * scale.Y);
}

void ACPlayer::PlayerTurn(const FInputActionValue& InValue)
{
	FVector2d scale = InValue.Get<FVector2d>();

	AddControllerPitchInput(scale.Y);
	AddControllerYawInput(scale.X);
}

void ACPlayer::PlayerJump(const FInputActionValue& InValue)
{
	Jump();
}

void ACPlayer::PlayerRun(const FInputActionValue& InValue)
{
	if (GetCharacterMovement()->MaxWalkSpeed > SpeedJog)
		GetCharacterMovement()->MaxWalkSpeed = SpeedJog;
	else
		GetCharacterMovement()->MaxWalkSpeed = SpeedRun;
}

void ACPlayer::PlayerDash(const FInputActionValue& InValue)
{
    UE_LOG(LogTemp, Warning, TEXT(">>> Dash"));
}

