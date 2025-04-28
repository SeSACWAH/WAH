#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CPlayer.generated.h"


UCLASS()
class WAH_API ACPlayer : public ACharacter
{
    GENERATED_BODY()

public:
    ACPlayer();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    virtual void PossessedBy(AController* NewController) override;

#pragma region Player State
protected:
    UPROPERTY(EditAnywhere, Category = Spawn)
    FVector SpawnPoint = FVector::ZeroVector;
    UPROPERTY(EditAnywhere, Category = Spawn)
    FRotator SpawnRotation = FRotator::ZeroRotator;

    FTimerHandle DamageTimer;
    FTimerHandle RecoverTimer;

    UPROPERTY(Replicated)
    bool bIsCody = false;

    int32 MaxHP = 12;
    UPROPERTY(ReplicatedUsing = OnRep_HP)
    int32 HP = MaxHP;

    UPROPERTY(EditAnywhere, Category = UI)
    TSubclassOf<class UCBattleUI> BattleWidget;

    UPROPERTY()
	class UCBattleUI* BattleUI;

    UFUNCTION(Server, Reliable)
    void ServerRPC_UpdateUIHP();

    UFUNCTION(NetMulticast, Reliable)
    void MulticastRPC_UpdateUIHP();

    virtual void InItBattleWidget();


    UPROPERTY(Replicated)
    bool bIsDamaged = false;
    UPROPERTY(Replicated)
    bool bIsDead = false;
    UPROPERTY(Replicated)
    bool bIsReviving = false;
    UPROPERTY(Replicated)
    bool bIsRevivalInputEntered = false;
    UPROPERTY(Replicated)
    bool bIsGodMode = false;

    UPROPERTY(EditDefaultsOnly, Category = Revival)
    float DamageDurationTime = 10.f;
    UPROPERTY(EditDefaultsOnly, Category = Revival)
    float RecoverTime = 0.2f;

    UPROPERTY(EditDefaultsOnly, Category = Revival)
    float RevivalTime = 7.f;
    UPROPERTY(EditDefaultsOnly, Category = Revival)
    int32 ReviveBoostAmount = 10;

    UPROPERTY(Replicated)
    float CurrentReviveTime = 0;

    UPROPERTY(EditDefaultsOnly, Category = Revival)
    float GodModeTime = 5.f;
    UPROPERTY(EditDefaultsOnly, Category = Revival)
    float CurrentGodModeTime = 0;

    UPROPERTY(EditDefaultsOnly, Category = Input)
    class UInputAction* IA_Revival;

    void RecoverHP();
    void RevivalInputEntered(const struct FInputActionValue& InValue);
    UFUNCTION(Server, Reliable)
    void ServerRPC_RevivalInputEntered();
    void GodMode(float InDeltaTime);

    virtual void OnDead();
    virtual void OnRevive(float InDeltaTime);
    UFUNCTION(Server, Reliable)
    void ServerRPC_ReviveInputEntered(float InDeltaTime);
    UFUNCTION(NetMulticast, Reliable)
    void MulticastRPC_ReviveInputEntered(float InDeltaTime);

    UFUNCTION()
    void OnRep_HP();

    UFUNCTION(NetMulticast, Reliable)
    void MulticastRPC_Dead();

    UFUNCTION(NetMulticast, Reliable)
    void MulticastRPC_Revive();
public:
    bool GetIsDead(){ return bIsDead; }

    UFUNCTION(Server, Reliable)
    void ServerRPC_OnDamaged(int32 InDamage);
    virtual void OnDamaged(int32 InDamage);
#pragma endregion

#pragma region Getters
public:
    int32 GetHp() const { return HP; }
    bool GetIsDamaged() const { return bIsDamaged; }
    bool GetIsDead() const { return bIsDead; }
#pragma endregion

#pragma region Setters
protected:
    void SetHP(int32 InHP) { HP += InHP; }
    void SetIsDamaged(bool InResult) { bIsDamaged = InResult; }
    void SetIsDead(bool InResult) { bIsDead = InResult; }
#pragma endregion

#pragma region Camera
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite ,Category = Camera)
    class USpringArmComponent* CameraBoom;
    UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = Camera)
    class UCameraComponent* PlayerCamear;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
    class USceneCaptureComponent2D* SceneCapture2D;

protected:
    UPROPERTY(EditDefaultsOnly, Category = Camera)
    float ArmLengthDefault = 400.f;

    FVector CameraBoomLocationDefault = {0, 0, 140};
#pragma endregion

#pragma region IMC
    UPROPERTY(EditDefaultsOnly, Category = Input)
    class UInputMappingContext* IMC_Player;
#pragma endregion


#pragma region Move
    UPROPERTY(EditDefaultsOnly, Category = Input)
    class UInputAction* IA_Move;

    UPROPERTY(EditDefaultsOnly, Category = Input)
    class UInputAction* IA_Turn;

    UPROPERTY(EditDefaultsOnly, Category = Input)
    class UInputAction* IA_Jump;

    UPROPERTY(EditDefaultsOnly, Category = Input)
    class UInputAction* IA_Run;

    // Turn
    UPROPERTY(EditDefaultsOnly, Category = Move)
    float MinPitchDefault = -78.f;

    UPROPERTY(EditDefaultsOnly, Category = Move)
    float MaxPitchDefault = 50;

    UPROPERTY(EditDefaultsOnly, Category = Move)
    float MinPitchRevival = MinPitchDefault * 0.5;

    UPROPERTY(EditDefaultsOnly, Category = Move)
    float MaxPitchRevival = MaxPitchDefault * 0.5;

    UPROPERTY(EditDefaultsOnly, Category = Move)
    float MouseSensitivityDefault = 0.85;

    // Speed
    UPROPERTY(EditDefaultsOnly, Category = Move)
    float SpeedJog = 500.f;

    UPROPERTY(EditDefaultsOnly, Category = Move)
    float SpeedRun = 800.f;

    // Jump
public:
    UPROPERTY(Replicated)
	bool bIsFalling;

	UPROPERTY(Replicated)
	int32 PlayerJumpCurrentCount;

protected:
    void DoMove(const struct FInputActionValue& InValue);

    void DoTurn(const struct FInputActionValue& InValue);

    void DoJump(const struct FInputActionValue& InValue);

    UFUNCTION(Server, Reliable)
    void ServerRPC_UpdateJumpInfo();

    UFUNCTION(NetMulticast, Reliable)
    void MulticastRPC_UpdateJumpInfo();

    UFUNCTION(Server, Reliable)
    void ServerRPC_DoJump();

    UFUNCTION(NetMulticast, Reliable)
    void MulticastRPC_DoJump();

    void DoRun(const struct FInputActionValue& InValue);

#pragma endregion

#pragma region Dash
    UPROPERTY(EditDefaultsOnly, Category = Input)
    class UInputAction* IA_Dash;

    UPROPERTY(Replicated)
    bool bCanDash = false;

    UPROPERTY(Replicated)
    bool bCanResetDash = false;

    FVector DashStartPos;
    FVector DashEndPos;

    FVector PrevPos;

    UPROPERTY(EditDefaultsOnly, Category = Dash)
    float DashDistance = 300.f;

    float DashCurrentTime = 0.f;

    UPROPERTY(EditDefaultsOnly, Category = Dash)
    float DashDurationTime = 0.2f;

    UPROPERTY(EditDefaultsOnly, Category = Dash)
    float DashCoolDownTime = 0.7f;

    ////// �߰�
public:
    bool GetCanDash() { return bCanDash; }
protected:
    virtual void StartDash(const FInputActionValue& InValue);
    ////// �߰� ��

    UFUNCTION(Server, Reliable)
    void ServerRPC_StartDash();
    UFUNCTION(NetMulticast, Reliable)
    void MulticastRPC_UpdateCanDash(bool InResult);
    void DoDash(float InDeltaTime);
    void ResetDash(float InDeltaTime);
#pragma endregion

#pragma region Aim
    UPROPERTY(EditDefaultsOnly, Category = Input)
    class UInputAction* IA_Aim;

    // CameraBoom
    UPROPERTY(EditDefaultsOnly, Category = Camera)
    float ArmLengthAim = 300.f;

    FVector CameraBoomLocationZoomIn = { 100, 100, 110 };

    // Time
    UPROPERTY(EditDefaultsOnly, Category = Aim)
    float ZoomCurrentTime = 0.f;

    UPROPERTY(EditDefaultsOnly, Category = Aim)
    float AimZoomMaxTime = 0.3f;

    // Mouse Sensitivity
    UPROPERTY(EditDefaultsOnly, Category = Aim)
    float MouseSensitivityAim = MouseSensitivityDefault * 0.2f;
    
    UPROPERTY(Replicated)
    bool bCanAim = false;   // Aim input�� ���Դ��� üũ
    UPROPERTY(Replicated)
    bool bCanZoom = false;

public:
    bool GetCanAim(){ return bCanAim; }
protected:
    // target�� Aim�� �����Ǿ����� üũ
    bool bIsAimLocked = false;

    // UI
    UPROPERTY(EditDefaultsOnly, Category = UI)
    TSubclassOf<class UCUnlockedCrossHairUI> UnlockedCrossshairWidget;

    UPROPERTY()
	class UCUnlockedCrossHairUI* UnlockedCrossshairUI;

    UPROPERTY(EditDefaultsOnly, Category = UI)
    TSubclassOf<class UCLockedCrossHairUI> LockedCrossshairWidget;

    UPROPERTY()
	class UCLockedCrossHairUI* LockedCrossshairUI;

    // Sphere Trace
    UPROPERTY(EditDefaultsOnly, Category = "Aim|SphereTrace")
    float SphereTraceDistance = 10000.f;

    UPROPERTY(EditDefaultsOnly, Category = "Aim|SphereTrace")
    float SphereTraceRadius = 10.f;

    UPROPERTY(Replicated)
    FVector FireDestination = FVector::ZeroVector;

    virtual void InitCrosshairWidgets();

    virtual void SetUnlockedCrosshairVisibility(bool bVisible);
    virtual void SetLockedCrosshairVisibility(bool bVisible);

    float EaseInOutQuad(float InRatio);
    float EaseOutExpo(float InRatio);
    float EaseOutSine(float InRatio);

    virtual void StartAim(const FInputActionValue& InValue);
    virtual void AdjustTargetArmLocation(float InDeltaTime);
    virtual void TriggerAim(const FInputActionValue& InValue);
    virtual void CompleteAim(const FInputActionValue& InValue);
#pragma endregion

#pragma region Gun
    virtual void AttachGun();
#pragma endregion

#pragma region Fire
    UPROPERTY(EditDefaultsOnly, Category = Input)
    class UInputAction* IA_Fire;

    virtual void DoFire();
#pragma endregion

#pragma region Network
    void PrintNetLog();
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
#pragma endregion

#pragma region TEST
    // TEST
    UPROPERTY(Replicated)
    float DebugReviveTime = 0;
    UPROPERTY(EditDefaultsOnly, Category = Input)
    class UInputAction* IA_TestDamage;
    UPROPERTY(EditDefaultsOnly, Category = Input)
    class UInputAction* IA_TestRevival;
    void TestDamage(const struct FInputActionValue& InValue);
    void TestRevival(const struct FInputActionValue& InValue);
#pragma endregion

};
