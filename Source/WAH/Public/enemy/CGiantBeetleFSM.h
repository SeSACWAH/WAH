// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CGiantBeetleFSM.generated.h"


UENUM(BlueprintType)
enum class EBeetleState : uint8
{
	Idle,
	Retarget,
	Charge,
	JumpToTarget,
	TripleJump,
	Damaged,
	Die
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WAH_API UCGiantBeetleFSM : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCGiantBeetleFSM();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = FSM)
	EBeetleState mState = EBeetleState::Idle;

	void IdleState();
	void RetargetState();
	void ChargeState();
	void JumpToTargetState();
	void TripleJumpState();
	void DamagedState();
	void DieState();

	void Stomp();

	UPROPERTY(EditDefaultsOnly, Category = FSM)
	float IdleDelayTime = 1.0f;

	float curTime = 0.0f;
};
