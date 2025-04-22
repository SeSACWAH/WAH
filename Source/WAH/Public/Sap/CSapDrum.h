// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CSapDrum.generated.h"

UCLASS()
class WAH_API ACSapDrum : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACSapDrum();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UCapsuleComponent* RootComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* MeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxSapGage = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SapGauge = 0;

	void Explosion();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UNiagaraSystem* ExplosionSys;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UNiagaraComponent* ExplosionComp;

	float ExplosionRadius = 300;

	UFUNCTION()
	void OnSapDrumOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
