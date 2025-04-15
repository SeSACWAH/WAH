// Fill out your copyright notice in the Description page of Project Settings.


#include "enemy/CTestEnemy.h"
#include "Player/CPlayer.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Components/BoxComponent.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Components/StaticMeshComponent.h"

// Sets default values
ACTestEnemy::ACTestEnemy()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	RootComponent = BoxComp;
	BoxComp->SetCollisionProfileName(FName("TestEnemy"));
	BoxComp->SetRelativeScale3D(FVector(2, 4, 2));
	BoxComp->OnComponentBeginOverlap.AddDynamic(this, &ACTestEnemy::OnTestEnemyOverlap);

	BoxMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoxMesh"));
	BoxMesh->SetupAttachment(BoxComp);
	ConstructorHelpers::FObjectFinder<UStaticMesh> tmpMesh(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	if(tmpMesh.Succeeded()) BoxMesh->SetStaticMesh(tmpMesh.Object);
	BoxMesh->SetCollisionProfileName(FName("NoCollision"));
}

// Called when the game starts or when spawned
void ACTestEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACTestEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACTestEnemy::OnTestEnemyOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Error, TEXT("################################ "));
	ACPlayer* player = Cast<ACPlayer>(OtherActor);

	if (player)
	{
        UE_LOG(LogTemp, Error, TEXT("####### ATTACK PLAYER ####### "));
		player->OnDamaged(6);
	}
}

