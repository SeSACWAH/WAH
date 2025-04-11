#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CGun.generated.h"

UCLASS()
class WAH_API ACGun : public AActor
{
	GENERATED_BODY()
	
public:	
	ACGun();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

protected:
	// Gun Mesh
    UPROPERTY(EditDefaultsOnly, Category = "Gun")
	class UStaticMeshComponent* GunMeshComp;

	// Bullets
	int32 BulletFXNum = 6;

	UPROPERTY(EditDefaultsOnly, Category = Bullet)
	TArray<class ACBullet*> Bullets;

	// Spawn Effect
	int32 FireFXNum = 6;

    UPROPERTY(EditDefaultsOnly, Category = Gun)
    TArray<class UNiagaraSystem*> FireFXSystems;

	UPROPERTY(EditDefaultsOnly, Category = Gun)
	TArray<class UNiagaraComponent*> FireFXPool;

	void InitializeFireFXSystem();
	void InitializeFireFXPool();
	void PlayFireFX();
	void OnFireFXFinished(UNiagaraComponent* InComp);
};
