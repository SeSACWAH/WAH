// Fill out your copyright notice in the Description page of Project Settings.


#include "enemy/CGiantBeetle.h"
#include "enemy/CGiantBeetleFSM.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Player/CPlayer.h"

// Sets default values
ACGiantBeetle::ACGiantBeetle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	fsm = CreateDefaultSubobject<UCGiantBeetleFSM>(TEXT("FSM"));
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	CapComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	SetRootComponent(CapComp);

	AttackBox = CreateDefaultSubobject<UBoxComponent>(TEXT("AttackBox"));
	AttackBox->SetupAttachment(RootComponent);
	//AttackBox->SetVisibility(false);
	AttackBox->SetCollisionProfileName(TEXT("AttackBox"));
	AttackBox->OnComponentBeginOverlap.AddDynamic(this, &ACGiantBeetle::OnAttackBoxOverlap);
}

// Called when the game starts or when spawned
void ACGiantBeetle::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACGiantBeetle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACGiantBeetle::OnAttackBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto player = Cast<ACPlayer>(OtherActor);
	if (player)
	{
		UE_LOG(LogTemp, Warning, TEXT("BOverLap!"));
		player->OnDamaged(12);
		bKill = true;
	}
}

