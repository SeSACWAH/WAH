#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../../../../../../../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h"
#include "../../../../../../../Plugins/FX/Niagara/Source/Niagara/Classes/NiagaraSystem.h"
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

	// Spawn Effect
	int32 FireFXNum = 6;

    UPROPERTY(EditDefaultsOnly, Category = Gun)
    TArray<UNiagaraSystem*> FireFXSystems;

	UPROPERTY(EditDefaultsOnly, Category = Gun)
	TArray<UNiagaraComponent*> FireFXPool;


	void InitializeFireFXSystem();
	void InitializeFireFXPool();
	void PlayFireFX();
	void OnFireFXFinished(UNiagaraComponent* InComp);
};
