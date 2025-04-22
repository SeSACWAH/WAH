#pragma once

#include "CoreMinimal.h"
#include "Guns/CBullet.h"
#include "CMatchBullet.generated.h"


UCLASS()
class WAH_API ACMatchBullet : public ACBullet
{
	GENERATED_BODY()
	
public:
	ACMatchBullet();

	UFUNCTION()
	void OnMatchBulletOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(ReplicatedUsing = OnRep_ActivateBullet)
	bool bIsActivated = false;
	UFUNCTION()
	void OnRep_ActivateBullet();

public:
	virtual USphereComponent* GetBulletComp() const override { return BulletComp; };
	virtual UStaticMeshComponent* GetBulletMesh() const override { return BulletMesh; }
	virtual void SetFireDestination(FVector InDirection) override { FireDestination = InDirection; }
	virtual void SetCanMove(bool InResult) override { bCanMove = InResult; }

	virtual void ActivateBullet(bool bIsActivate) override;
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPC_ActivateBullet(bool bIsActivate);
	virtual void DoMoveBullet(float InDeltaTime) override;
	virtual void CompleteMoveBullet(FVector InDestination) override;
};
