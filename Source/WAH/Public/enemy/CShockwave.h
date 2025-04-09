// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CShockwave.generated.h"

UCLASS()
class WAH_API ACShockwave : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACShockwave();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UStaticMeshComponent* RingComp;

	FTimerHandle ExpandTimer;

	float CurTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ExpandTime = 5.0f;
};
