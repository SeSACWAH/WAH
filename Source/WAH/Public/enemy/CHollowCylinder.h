// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CHollowCylinder.generated.h"

UCLASS()
class WAH_API ACHollowCylinder : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACHollowCylinder();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cylinder")
    float OuterRadius = 50.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cylinder")
    float InnerRadius = 30.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cylinder")
    float Height = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cylinder")
    int32 Segments = 32;

    UFUNCTION(BlueprintCallable, Category = "Cylinder")
    void GenerateCylinder();

	UPROPERTY(VisibleAnywhere)
	class UProceduralMeshComponent* ProceduralMeshComp;

	//UFUNCTION()

};
