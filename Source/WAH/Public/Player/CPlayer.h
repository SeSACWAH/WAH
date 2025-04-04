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

	UPROPERTY(EditDefaultsOnly, Category = Input)
	class UInputMappingContext* IMC_Player;

#pragma region Move
	UPROPERTY(EditDefaultsOnly, Category = Input)
	class UInputAction* IA_Move;

	UPROPERTY(EditDefaultsOnly, Category = Input)
	class UInputAction* IA_Turn;

	UPROPERTY(EditDefaultsOnly, Category = Input)
	float SpeedMove;
	
	UPROPERTY(EditDefaultsOnly, Category = Input)
	float SpeedRun;

	void Move(const struct FInputActionValue& InValue);

	void Turn(const struct FInputActionValue& InValue);
#pragma endregion

};
