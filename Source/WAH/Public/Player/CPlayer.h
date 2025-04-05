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

	UPROPERTY(EditDefaultsOnly, Category = Input)
	float SpeedJog;
	
	UPROPERTY(EditDefaultsOnly, Category = Input)
	float SpeedRun;

	void PlayerMove(const struct FInputActionValue& InValue);

	void PlayerTurn(const struct FInputActionValue& InValue);

	void PlayerJump(const struct FInputActionValue& InValue);

	void PlayerRun(const struct FInputAcitonValue& InValue);

#pragma endregion

#pragma region Dash
	UPROPERTY(EditDefaultsOnly, Category = Input)
	class UInputAction* IA_Dash;

	UPROPERTY(EditDefaultsOnly, Category = Input)
	float DistanceDash = 200.f;

	void PlayerDash(const struct FInputAcitonValue& InValue);
#pragma endregion
};
