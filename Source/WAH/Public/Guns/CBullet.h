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

	UPROPERTY(Replicated)
	bool bCanMove = false;

	UPROPERTY(Replicated)
	FVector FireDestination;

	FTimerHandle DeactivateTimer;

    //UFUNCTION()
    //virtual void OnMatchBulletOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
public:
    virtual USphereComponent* GetBulletComp() const;
    virtual UStaticMeshComponent* GetBulletMesh() const;
    virtual void SetFireDestination(FVector InDirection);
	virtual void SetCanMove(bool InResult);

	virtual void ActivateBullet(bool bIsActivate);
	virtual void DoMoveBullet(float InDeltaTime);
	virtual void CompleteMoveBullet(FVector InDestination);

#pragma region Network
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
#pragma endregion
};
