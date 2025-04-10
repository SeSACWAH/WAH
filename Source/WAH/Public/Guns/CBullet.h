#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CBullet.generated.h"

UCLASS()
class WAH_API ACBullet : public AActor
{
	GENERATED_BODY()
	
public:	
	ACBullet();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

private:
    UPROPERTY(EditDefaultsOnly, Category = "Bullet")
	class USphereComponent* Bullet;

	UPROPERTY(EditDefaultsOnly, Category = "Bullet")
	int32 LineSmooth = 40;

	TArray<FVector> Lines;

	UPROPERTY(EditDefaultsOnly, Category = "Bullet")
	float BulletForce = 1500.f;

	UPROPERTY(EditDefaultsOnly, Category = "Bullet")
	float BulletGravity = -3000;

	UPROPERTY(EditDefaultsOnly, Category = "Bullet")
	float BulletSimulateTime = 0.02f;

	void MoveBullet(FVector InStartPos, FVector InEndPos)
};
