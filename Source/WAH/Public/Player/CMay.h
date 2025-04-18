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
    UPROPERTY(EditAnywhere, Category = Gun)
    TSubclassOf<class ACMatchGun> MatchBP;

#pragma region Gun
    UPROPERTY()
    class ACMatchGun* MatchGun;

    virtual void AttachGun() override;
#pragma endregion

#pragma region Trigger Aim
    virtual void InitCrosshairWidgets() override;
    virtual void SetUnlockedCrosshairVisibility(bool bVisible);
    virtual void SetLockedCrosshairVisibility(bool bVisible);

    virtual void StartAim(const FInputActionValue& InValue);
    virtual void AdjustTargetArmLocation(float InDeltaTime);
    virtual void TriggerAim(const FInputActionValue& InValue) override;
    virtual void CompleteAim(const FInputActionValue& InValue);
#pragma endregion

#pragma region Status
    virtual void OnDead() override;
    virtual void OnRevive(float InDeltaTime);
#pragma endregion
};
