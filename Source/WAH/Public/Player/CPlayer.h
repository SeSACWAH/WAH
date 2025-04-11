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

private:

#pragma region Player Info
    int32 HP = 100;
    bool bIsDamaged = false;

    //int32 MaxBulletCount = 3;   // for May
    //float MaxSapAmount = 100.f; // for Cody
#pragma endregion

#pragma region Camera
    UPROPERTY(VisibleAnywhere, Category = Camera)
    class UCameraComponent* PlayerCamear;

    UPROPERTY(VisibleAnywhere, Category = Camera)
    class USpringArmComponent* CameraBoom;

    UPROPERTY(EditDefaultsOnly, Category = Camera)
    float ArmLengthDefault = 400.f;

    FVector CameraBoomLocationDefault = {0, 0, 140};
#pragma endregion

#pragma region IMC
    UPROPERTY(EditDefaultsOnly, Category = Input)
    class UInputMappingContext* IMC_Player;
#pragma endregion

#pragma region Gun
    UPROPERTY(EditAnywhere, Category = Gun)
    TSubclassOf<class ACGun> GunBP;

    UPROPERTY()
    class ACGun* Gun;

    void AttachGun();
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
    float MinPitch = -78.f;

    UPROPERTY(EditDefaultsOnly, Category = Move)
    float MaxPitch = 50;

    UPROPERTY(EditDefaultsOnly, Category = Move)
    float MouseSensitivityDefault = 0.85;

    // Speed
    UPROPERTY(EditDefaultsOnly, Category = Move)
    float SpeedJog = 600.f;

    UPROPERTY(EditDefaultsOnly, Category = Move)
    float SpeedRun = 900.f;

    void DoMove(const struct FInputActionValue& InValue);

    void DoTurn(const struct FInputActionValue& InValue);

    void DoJump(const struct FInputActionValue& InValue);

    void DoRun(const struct FInputActionValue& InValue);

#pragma endregion

#pragma region Zoom
    template <typename T>
    T Zoom(T InStartVal, T InEndVal, float InRatio);
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
    float DashCoolDownTime = 1.f;

    void StartDash(const FInputActionValue& InValue);
    void DoDash(float InDeltaTime);
    void ResetDash(float InDeltaTime);
#pragma endregion

#pragma region Aim
    /*  Aim에서 구현해야 할 것
    - UnlockedCrosshairUI가 정중앙에 떠 있다
        - UnlockedCrosshairUI->SetVisibility(true)
        - LockedCrossshairUI->SetVisibility(false)
    - Zoom in이 실행된다 - Camera Boom의 길이가 짧아지게 구현
    - 마우스 감도가 낮아진다 - Turn이 느리게 바뀌도록 구현
    - 카메라 위치가 바뀐다 - Camera / Camera Boom의 Location이 바뀌도록 구현
    - damage 상태면 안보임

    여기서부터 May랑 Cody가 다름
    - 타겟에 닿으면 활성화 AimedCrosshairUI가 뜸
        - UnlockedCrosshairUI->SetVisibility(false)
        - LockedCrossshairUI->SetVisibility(true)
    - 일정 범위 안에 들어왔으면 Aim이 활성화된다
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
    
    bool bCanAim = false;   // Aim input이 들어왔는지 체크
    bool bCanZoom = false;

    //bool bCanAdjustTargetArmLength = false;

    // target에 Aim이 고정되었는지 체크
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

    void InitCrosshairWidgets();

    void SetUnlockedCrosshairVisibility (bool bVisible);
    void SetLockedCrosshairVisibility (bool bVisible);

    float EaseInOutQuad(float InRatio);
    float EaseOutExpo(float InRatio);
    float EaseOutSine(float InRatio);

    void StartAim(const FInputActionValue& InValue);
    void AdjustTargetArmLocation(float InDeltaTime);
    void CompleteAim();
#pragma endregion
};
