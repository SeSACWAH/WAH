// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CCody.h"
#include "Guns/CGun.h"
#include "Guns/CSapGun.h"
#include "Engine/GameViewportClient.h"
#include "GameMapsSettings.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "EngineUtils.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/CCodyAnim.h"

ACCody::ACCody()
{
	ConstructorHelpers::FClassFinder<ACSapGun> tempGun(TEXT("/Script/Engine.Blueprint'/Game/KHH/Blueprints/BP_SapGun.BP_SapGun_C'"));
	if (tempGun.Succeeded()) GunBP = tempGun.Class;
	
	SpawnPoint = FVector(-100, 100, 0);
	SpawnRotation = FRotator(0, -180, 0);
	//SetActorLocationAndRotation(SpawnPoint, SpawnRotation);
}

void ACCody::BeginPlay()
{
	Super::BeginPlay();
	bIsCody = true;
	if (HasAuthority())
	{
		Gun = GetWorld()->SpawnActor<ACGun>(GunBP);
		if (Gun) Gun->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("GunSocket"));
	}
}

void ACCody::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (HasAuthority())
	{
		APlayerCameraManager* playerCM = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
		auto pc = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());
		if (playerCM)
		{
			CameraBoomRotation = playerCM->GetCameraRotation();
			CameraBoomPosition = playerCM->GetCameraLocation();
		}
		
		MulticastRPC_UpdateCaptureRotation(CameraBoomPosition, CameraBoomRotation);
	}
}

void ACCody::DoFire()
{
	if(bIsDead || bIsReviving) return;

	if(!bCanAim) return;
	ServerRPC_Fire();
}

void ACCody::ServerRPC_Fire_Implementation()
{
	if (Gun)
	{
		Gun->FireBullet(FireDest);
	}
}

void ACCody::OnDead()
{
	Super::OnDead();
	ServerRPC_SetGun(true);
}

void ACCody::ServerRPC_SetGun_Implementation(bool bVisible)
{
	Gun->SetActorHiddenInGame(bVisible);
}

void ACCody::OnRevive(float InDeltaTime)
{
	Super::OnRevive(InDeltaTime);
	if (CurrentReviveTime >= RevivalTime)
	{
		ServerRPC_SetGun(false);
	}
}


void ACCody::StartAim(const FInputActionValue& InValue)
{
	if(bIsDead || bIsDamaged || bIsReviving) return;
	ServerRPC_StartAim();
}

void ACCody::ServerRPC_StartAim_Implementation()
{
	MulticastRPC_StartAim();
}

void ACCody::MulticastRPC_StartAim_Implementation()
{
	bCanAim = true;
	bCanZoom = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
}

void ACCody::AdjustTargetArmLocation(float InDeltaTime)
{
	float ratio;

	if (bCanZoom) ZoomCurrentTime += InDeltaTime;
	else ZoomCurrentTime -= InDeltaTime;

	// CurTime이 범위를 벗어나는 것을 한정해줌
	ZoomCurrentTime = FMath::Clamp(ZoomCurrentTime, 0, AimZoomMaxTime);

	ratio = EaseInOutQuad(ZoomCurrentTime / AimZoomMaxTime);
	CameraBoom->SetRelativeLocation(FMath::Lerp(CameraBoomLocationDefault, CameraBoomLocationZoomIn, ratio));
}

void ACCody::CompleteAim(const FInputActionValue& InValue)
{
	ServerRPC_CompleteAim();
}

void ACCody::ServerRPC_CompleteAim_Implementation()
{
	MulticastRPC_CompleteAim();
}

void ACCody::MulticastRPC_CompleteAim_Implementation()
{
	bCanAim = false;
	bCanZoom = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

void ACCody::StartDash(const FInputActionValue& InValue)
{
	Super::StartDash(InValue);

	ServerRPC_PlayDashAnim();
}

void ACCody::ServerRPC_PlayDashAnim_Implementation()
{
	MulticastRPC_PlayDashAnim();
}

void ACCody::MulticastRPC_PlayDashAnim_Implementation()
{
	auto anim = Cast<UCCodyAnim>(GetMesh()->GetAnimInstance());
	anim->PlayDashAnimation();
}

void ACCody::OnRep_CameraBoomRotation()
{
	if (SceneCapture2D)
	{
		SceneCapture2D->SetRelativeRotation(CameraBoomRotation);
	}
}

void ACCody::OnRep_CameraBoomPosition()
{
	if (SceneCapture2D)
	{
		SceneCapture2D->SetRelativeRotation(CameraBoomRotation);
	}
}

void ACCody::MulticastRPC_UpdateCaptureRotation_Implementation(FVector loc, FRotator rot)
{
	if (SceneCapture2D)
	{
		SceneCapture2D->SetRelativeLocationAndRotation(loc , rot);
	}
}

void ACCody::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACCody, CameraBoomRotation);
	DOREPLIFETIME(ACCody, CameraBoomPosition);
}
