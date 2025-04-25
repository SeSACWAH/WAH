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
		auto pc = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());
		if (pc)
		{
			//CameraBoomRotation = pc->GetControlRotation();
		}
		
		//MulticastRPC_UpdateCaptureRotation(CameraBoomRotation);
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


void ACCody::OnRep_CameraBoomRotation()
{
	//if (SceneCapture2D)
	//{
	//	SceneCapture2D->SetRelativeRotation(CameraBoomRotation);
	//}
}

void ACCody::MulticastRPC_UpdateCaptureRotation_Implementation(FRotator rot)
{
	//if (CameraBoom)
	//{
	//	CameraBoom->SetRelativeRotation(rot);
	//}
}

void ACCody::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACCody, CameraBoomRotation);
}
