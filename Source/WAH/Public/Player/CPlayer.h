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
#pragma region Camera
    UPROPERTY(VisibleAnywhere, Category = Camera)
    class UCameraComponent* PlayerCamear;

    UPROPERTY(VisibleAnywhere, Category = Camera)
    class USpringArmComponent* CameraBoom;
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
    UPROPERTY(EditDefaultsOnly)
    float MinPitch = -90.f;

    UPROPERTY(EditDefaultsOnly)
    float MaxPitch = 30;

    UPROPERTY(EditDefaultsOnly, Category = Move)
    float SpeedJog = 600.f;

    UPROPERTY(EditDefaultsOnly, Category = Move)
    float SpeedRun = 900.f;

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

    UPROPERTY(EditDefaultsOnly, Category = Input)
    float DashDurationTime = 0.2f;

    UPROPERTY(EditDefaultsOnly, Category = Input)
    float DashCoolDownTime = 1.f;

    void StartDash(const FInputActionValue& InValue);
    void DoDash(float InDeltaTime);
    void ResetDash(float InDeltaTime);
#pragma endregion

};
