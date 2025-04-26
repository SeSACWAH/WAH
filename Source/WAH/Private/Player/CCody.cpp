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

ACCody::ACCody()
{
	ConstructorHelpers::FClassFinder<ACSapGun> tempGun(TEXT("/Script/Engine.Blueprint'/Game/KHH/Blueprints/BP_SapGun.BP_SapGun_C'"));
	if (tempGun.Succeeded()) GunBP = tempGun.Class;
	
	//SpawnPoint = FVector(-100, -100, 0);
	//SpawnRotation = FRotator(0, -180, 0);
	//SetActorLocationAndRotation(SpawnPoint, SpawnRotation);
}

void ACCody::BeginPlay()
{
	Super::BeginPlay();
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
	bCanAim = true;
	bCanZoom = true;
}

void ACCody::ServerRPC_StartAim_Implementation()
{
	MulticastRPC_StartAim();
}

void ACCody::MulticastRPC_StartAim_Implementation()
{
	GetCharacterMovement()->bOrientRotationToMovement = false;
}

void ACCody::CompleteAim(const FInputActionValue& InValue)
{
	ServerRPC_CompleteAim();
	bCanAim = false;
	bCanZoom = false;
}

void ACCody::ServerRPC_CompleteAim_Implementation()
{
	MulticastRPC_CompleteAim();
}

void ACCody::MulticastRPC_CompleteAim_Implementation()
{
	GetCharacterMovement()->bOrientRotationToMovement = true;
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
