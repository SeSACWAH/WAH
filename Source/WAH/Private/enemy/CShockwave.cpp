// Fill out your copyright notice in the Description page of Project Settings.


#include "enemy/CShockwave.h"

// Sets default values
ACShockwave::ACShockwave()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RingComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RingComp"));
}

// Called when the game starts or when spawned
void ACShockwave::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetTimerManager().SetTimer(ExpandTimer, FTimerDelegate::CreateLambda(
		[this]()
		{
			CurTime += GetWorld()->DeltaTimeSeconds;
			FVector curSclae = RingComp->GetRelativeScale3D();
			RingComp->SetRelativeScale3D(FVector(curSclae.X + 0.5, curSclae.Y + 0.5, curSclae.Z));
			if (CurTime >= ExpandTime)
			{
				GetWorldTimerManager().ClearTimer(ExpandTimer);
				Destroy();
			}
		}
	),0.02,true);
}

// Called every frame
void ACShockwave::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

