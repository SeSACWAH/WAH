#include "Player/CPlayer.h"
#include "../../../../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputSubsystems.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Camera/CameraComponent.h"
#include "../../../../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputComponent.h"
#include "../../../../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/InputMappingContext.h"

ACPlayer::ACPlayer()
{
	PrimaryActorTick.bCanEverTick = true;

	/* Camera */
	PlayerCamear = CreateDefaultSubobject<UCameraComponent>( TEXT("PlayerCamera") );
	PlayerCamear->SetupAttachment(RootComponent);


	/* IMC */
	ConstructorHelpers::FObjectFinder<UInputMappingContext> tmpIMC ( TEXT("/Script/EnhancedInput.InputMappingContext'/Game/DYL/Inputs/IMC_Player.IMC_Player'"));
	if( tmpIMC.Succeeded() ) IMC_Player = tmpIMC.Object;

	ConstructorHelpers::FObjectFinder<UInputAction> tmpIAMove ( TEXT("/Script/EnhancedInput.InputAction'/Game/DYL/Inputs/IA_Move.IA_Move'") );
	if( tmpIAMove.Succeeded() ) IA_Move = tmpIAMove.Object;
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
		inputSystem->BindAction(IA_Move, ETriggerEvent::Triggered, this, &ACPlayer::Move);
	}
}

void ACPlayer::Move(const struct FInputActionValue& InValue)
{
	FVector2D scale = InValue.Get<FVector2D>();
	FVector dir = PlayerCamear->GetComponentLocation() * scale.X + PlayerCamear->GetRightVector() * scale.Y;
	AddMovementInput(dir);
}

