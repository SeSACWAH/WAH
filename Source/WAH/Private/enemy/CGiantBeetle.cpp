// Fill out your copyright notice in the Description page of Project Settings.


#include "enemy/CGiantBeetle.h"
#include "enemy/CGiantBeetleFSM.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"

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
	AttackBox->SetVisibility(false);

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

