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

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Bullet")
	class USphereComponent* BulletComp;

	UPROPERTY(EditDefaultsOnly, Category = "Bullet")
	class UStaticMeshComponent* BulletMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Bullet")
	float BulletSpeed = 2000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Bullet")
	float BulletDieTime = 3.f;

	bool bCanMove = false;

	FVector FireDestination;

	FTimerHandle DeactivateTimer;

public:
	USphereComponent* GetBulletComp() const { return BulletComp; };
	UStaticMeshComponent* GetBulletMesh() const { return BulletMesh; }
	void SetFireDestination(FVector InDirection) { FireDestination = InDirection; }
	void SetCanMove(bool InResult) { bCanMove = InResult; }

	void ActivateBullet(bool bIsActivate);
	void DoMoveBullet(float InDeltaTime);
	void CompleteMoveBullet(FVector InDestination);

	UFUNCTION()
	void OnBulletOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
