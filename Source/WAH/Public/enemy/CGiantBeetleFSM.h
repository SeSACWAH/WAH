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
	UPROPERTY(VisibleAnywhere, Category = FSM)
	class UCBeetleAnim* Anim;

	UPROPERTY()
	class AAIController* ai;

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
	float ChargeSpeed = 1000;

	UPROPERTY(EditAnywhere, Category = FSM)
	float TJTime = 2.4f;

	float CurTJTime = 0.0f;

	UPROPERTY(EditAnywhere, Category = Jump)
	float JumpSpeed = 1500;

	float JumpTotTime;
	float JumpCurTime;

	float ChargeStompTime = 4.3;
	float ChargeAttackTime = 1.3;
	float ChargeCurtime = 0.0;
	float JumpAgroCurTime = 0.0;
	float JumpAgroTime = 1.7;
	float TJumpAgroCurTime = 0.0;
	float TJumpAgroTime = 2.5;
	float JumpLandTime = 1.67;
	float JumpCurLandTime = 0;

	UPROPERTY(EditAnywhere, Category = Jump)
	float JumpGravity = 980;

	float JumpVelocityZ = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Jump)
	float JumpMinDistance = 200;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Jump)
	float DamageDelayTime = 3.5;

	float CurDMGTime = 0;

	FVector JumpStartloc;
	FVector JumpEndloc;
	FVector JumpVelocity;


	UPROPERTY(EditDefaultsOnly, Category = FSM, Replicated)
	int32 MaxChargeCnt = 2;

	int32 ChargeCnt = 0;

	int32 TripleCnt = 0;

	int32 MaxHP = 100;

	UPROPERTY(Replicated)
	int32 HP = MaxHP;

	FVector TargetLoc;

	bool bWasTriple = true;
	bool bWasDamaged = false;

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

	UFUNCTION(Server, Unreliable)
	void ServerRPC_Stomp();

	UFUNCTION(NetMulticast, Unreliable)
	void MultiRPC_Stomp();
	
	UFUNCTION(Server, Unreliable)
	void ServerRPC_OnDamage(int32 damage);

	UFUNCTION(NetMulticast, Unreliable)
	void MultiRPC_ONDamage( int32 damage);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
