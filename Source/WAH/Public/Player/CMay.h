#pragma once

#include "CoreMinimal.h"
#include "Player/CPlayer.h"
#include "CMay.generated.h"

UCLASS()
class WAH_API ACMay : public ACPlayer
{
	GENERATED_BODY()

public:
    ACMay();
	
protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

#pragma region Fire
    int32 MaxBulletCount = 3;
    int32 CurrentBulletCount = MaxBulletCount;

    bool bIsInFireDelayTime = false;

    UPROPERTY(EditDefaultsOnly, Category = Fire)
    float FireDelayTime = 1.0f;

    UPROPERTY(EditDefaultsOnly, Category = Fire)
    float ChargeAmmoTime = 3.5f;

    virtual void DoFire() override;
#pragma endregion

#pragma region Trigger Aim
    virtual void TriggerAim(const FInputActionValue& InValue) override;
#pragma endregion
};
