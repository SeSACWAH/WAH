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

#pragma region Player State
protected:
    int32 MaxHP = 12;
    int32 HP = MaxHP;
    bool bIsDamaged = false;
    bool bIsDead = false;
    bool bIsReviving = false;

    float DamageDurationTime = 10.f;
    float RecoverTime = 0.2f;

    float CurrentReviveTime = 0;
    float RevivalTime = 7.f;

    void RecoverHP();
    void OnDead();
    void OnRevive(float InDeltaTime);
public:
    void OnDamaged(int32 InDamage);
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
private:
    UPROPERTY(VisibleAnywhere, Category = Camera)
    class UCameraComponent* PlayerCamear;

    UPROPERTY(VisibleAnywhere, Category = Camera)
    class USpringArmComponent* CameraBoom;

    UPROPERTY(EditDefaultsOnly, Category = Camera)
    float ArmLengthDefault = 400.f;

    FVector CameraBoomLocationDefault = {0, 0, 140};
#pragma endregion

#pragma region IMC
private:
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

    void DoMove(const struct FInputActionValue& InValue);

    void DoTurn(const struct FInputActionValue& InValue);

    void DoJump(const struct FInputActionValue& InValue);

    void DoRun(const struct FInputActionValue& InValue);

#pragma endregion

#pragma region Dash
    UPROPERTY(EditDefaultsOnly, Category = Input)
    class UInputAction* IA_Dash;

    bool bCanDash = false;
    bool bCanResetDash = false;

    FVector DashStartPos;
    FVector DashEndPos;

    UPROPERTY(EditDefaultsOnly, Category = Dash)
    float DashDistance = 300.f;

    float DashCurrentTime = 0.f;

    UPROPERTY(EditDefaultsOnly, Category = Dash)
    float DashDurationTime = 0.2f;

    UPROPERTY(EditDefaultsOnly, Category = Dash)
    float DashCoolDownTime = 0.7f;

    void StartDash(const FInputActionValue& InValue);
    void DoDash(float InDeltaTime);
    void ResetDash(float InDeltaTime);
#pragma endregion

#pragma region Aim
    /*  Aim���� �����ؾ� �� ��
    - UnlockedCrosshairUI�� ���߾ӿ� �� �ִ�
        - UnlockedCrosshairUI->SetVisibility(true)
        - LockedCrossshairUI->SetVisibility(false)
    - Zoom in�� ����ȴ� - Camera Boom�� ���̰� ª������ ����
    - ���콺 ������ �������� - Turn�� ������ �ٲ�� ����
    - ī�޶� ��ġ�� �ٲ�� - Camera / Camera Boom�� Location�� �ٲ�� ����
    - damage ���¸� �Ⱥ���
    - Sphere Trace�� �����Ѵ�
    - ���� Speed���� ���� Speed��ŭ ��������

    ���⼭���� May�� Cody�� �ٸ�
    - Ÿ�ٿ� ������ Ȱ��ȭ AimedCrosshairUI�� ��
        - UnlockedCrosshairUI->SetVisibility(false)
        - LockedCrossshairUI->SetVisibility(true)
    - ���� ���� �ȿ� �������� Aim�� Ȱ��ȭ�ȴ�
    */
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
    
    bool bCanAim = false;   // Aim input�� ���Դ��� üũ
    bool bCanZoom = false;

    //bool bCanAdjustTargetArmLength = false;

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

    FVector FireDestination;

    void InitCrosshairWidgets();

    void SetUnlockedCrosshairVisibility (bool bVisible);
    void SetLockedCrosshairVisibility (bool bVisible);

    float EaseInOutQuad(float InRatio);
    float EaseOutExpo(float InRatio);
    float EaseOutSine(float InRatio);

    void StartAim(const FInputActionValue& InValue);
    void AdjustTargetArmLocation(float InDeltaTime);
    void TriggerAim(const FInputActionValue& InValue);
    void CompleteAim(const FInputActionValue& InValue);
#pragma endregion

#pragma region Gun
    UPROPERTY(EditAnywhere, Category = Gun)
    TSubclassOf<class ACGun> GunBP;

    UPROPERTY()
    class ACGun* Gun;

    void AttachGun();
#pragma endregion

#pragma region Fire
    UPROPERTY(EditDefaultsOnly, Category = Input)
    class UInputAction* IA_Fire;

    // MAY
    int32 MaxBulletCount = 3;
    int32 CurrentBulletCount = MaxBulletCount;

    // CODY
    float MaxSapAmount = 1;
    float CurrentSapAmout = MaxSapAmount;

    bool bIsInFireDelayTime = false;

    UPROPERTY(EditDefaultsOnly, Category = Fire)
    float FireDelayTime = 1.0f;

    // MAY
    // MUST ChargeAmmoTime < FireDelayTime
    UPROPERTY(EditDefaultsOnly, Category = Fire)
    float ChargeAmmoTime = 3.5f;

    void DoFire();
#pragma endregion

};
