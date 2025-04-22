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

	UPROPERTY(VisibleAnywhere, Category = FSM, Replicated)
	class ACPlayer* Target;
	UPROPERTY(VisibleAnywhere, Category = FSM)
	class ACPlayer* Player1;
	UPROPERTY(VisibleAnywhere, Category = FSM)
	class ACPlayer* Player2;
	UPROPERTY(VisibleAnywhere, Category = FSM)
	class ACGiantBeetle* Me;

	//UPROPERTY()
	//class AAIController* ai;

	UPROPERTY(EditDefaultsOnly, Category = FSM)
	float IdleDelayTime = 1.0f;

	float CurIdleTime = 0.0f;

	UPROPERTY(EditAnywhere, Category = FSM)
	float RotTime = 1.0f;

	float CurRotTime = 0.0f;

	UPROPERTY(EditAnywhere, Category = FSM)
	float ChargeTime = 3.0f;

	float CurChargeTime = 0.0f;

	UPROPERTY(EditAnywhere, Category = FSM)
	float ChargeCurve = 0.05f;

	UPROPERTY(EditAnywhere, Category = FSM)
	float ChargeSpeed = 500;

	UPROPERTY(EditAnywhere, Category = FSM)
	float TJTime = 1.0f;

	float CurTJTime = 0.0f;

	UPROPERTY(EditAnywhere, Category = Jump)
	float JumpSpeed = 1500;

	float JumpTotTime;
	float JumpCurTime;

	UPROPERTY(EditAnywhere, Category = Jump)
	float JumpGravity = 980;

	float JumpVelocityZ = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Jump)
	float JumpMinDistance = 200;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Jump)
	float DamageDelayTime = 3;

	float CurDMGTime = 0;

	FVector JumpStartloc;
	FVector JumpEndloc;
	FVector JumpVelocity;


	UPROPERTY(EditDefaultsOnly, Category = FSM)
	int32 MaxChargeCnt = 2;

	int32 ChargeCnt = 0;

	int32 TripleCnt = 0;

	int32 MaxHP = 100;

	int32 HP = MaxHP;

	FVector TargetLoc;

	bool bWasTriple = true;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ACShockwave> ShockwaveFac;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ACHollowCylinder> ShockCylFac;
	
	void IdleState();
	void RetargetState();
	void ChargeState();
	void JumpToTargetState();
	void TripleJumpState();
	void DamagedState();
	void DieState();

	void Stomp();

	void OnDamageProcess(int32 damage);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
