// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CGiantBeetle.generated.h"

UCLASS()
class WAH_API ACGiantBeetle : public APawn
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACGiantBeetle();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UCapsuleComponent* CapComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBoxComponent* AttackBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = FSMComponent)
	class UCGiantBeetleFSM* fsm;

	float MaxHP = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	float CurHP = MaxHP;

	bool bKill = false;

	UFUNCTION()
	void OnAttackBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
};
