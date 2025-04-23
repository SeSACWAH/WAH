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
    UPROPERTY(Replicated)
    int32 CurrentBulletCount = MaxBulletCount;

    UPROPERTY(Replicated)
    bool bIsInFireDelayTime = false;

    UPROPERTY(EditDefaultsOnly, Category = Fire)
    float FireDelayTime = 1.0f;

    UPROPERTY(EditDefaultsOnly, Category = Fire)
    float ChargeAmmoTime = 3.5f;

    virtual void DoFire() override;
    UFUNCTION(Server, Reliable)
    void ServerRPC_DoFire();
#pragma endregion

#pragma region Gun
    UPROPERTY(EditAnywhere, Category = Gun)
    TSubclassOf<class ACMatchGun> MatchBP;

    UPROPERTY()
    class ACMatchGun* MatchGun;

    virtual void AttachGun() override;
#pragma endregion

#pragma region Trigger Aim
    UPROPERTY(Replicated)
    FVector AimPosition;
    UFUNCTION(Server, Reliable)
    void ServerRPC_GetAimPosition();

    virtual void InitCrosshairWidgets() override;
    virtual void SetUnlockedCrosshairVisibility(bool bVisible);
    virtual void SetLockedCrosshairVisibility(bool bVisible);

    virtual void StartAim(const FInputActionValue& InValue);
    UFUNCTION(Server, Reliable)
    void ServerRPC_StartAim();
    UFUNCTION(NetMulticast, Reliable)
    void MulticastRPC_StartAim();
    virtual void AdjustTargetArmLocation(float InDeltaTime);
    virtual void TriggerAim(const FInputActionValue& InValue) override;
    UFUNCTION(Server, Reliable)
    void ServerRPC_UpdateFireDestination(FVector InFireDestination);
    virtual void CompleteAim(const FInputActionValue& InValue);
    UFUNCTION(Server, Reliable)
    void ServerRPC_CompleteAim();
    UFUNCTION(NetMulticast, Reliable)
    void MulticastRPC_CompleteAim();
#pragma endregion

#pragma region Status
    virtual void OnDead() override;
    UFUNCTION(Server, Reliable)
    void ServerRPC_AdjustGunVisibilityAndCollision(bool bIsTrue);
    virtual void OnRevive(float InDeltaTime);
#pragma endregion

#pragma region Network
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
#pragma endregion
};
