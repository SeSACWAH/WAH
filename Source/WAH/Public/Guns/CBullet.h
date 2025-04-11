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
	float BulletSpeed = 1000.f;

	FVector StartPos;
	FVector EndPos;
	FVector Velocity;

public:
	void SetPositionAndVelocity(FVector InStartPos, FVector InEndPos);
};
