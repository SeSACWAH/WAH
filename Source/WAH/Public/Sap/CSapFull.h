#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CSapFull.generated.h"

UCLASS()
class WAH_API ACSapFull : public AActor
{
	GENERATED_BODY()
	
public:	
	ACSapFull();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = Sap)
	class USphereComponent* SapComp;

	UPROPERTY(EditDefaultsOnly, Category = Sap)
	class UStaticMeshComponent* SapMeshComp;

public:
	UFUNCTION()
	void OnSapFullOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(Server, Reliable)
	void ServerRPC_MatchBulletOverlap(class ACMatchBullet* InMatchBullet);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_AdjustSapFullLocationAndVisibility();
};
