// Fill out your copyright notice in the Description page of Project Settings.


#include "enemy/CGiantBeetle.h"

// Sets default values
ACGiantBeetle::ACGiantBeetle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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

