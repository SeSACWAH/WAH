// Fill out your copyright notice in the Description page of Project Settings.


#include "enemy/CGiantBeetleFSM.h"
#include "enemy/CGiantBeetle.h"
#include "Player/CPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "enemy/CShockwave.h"
#include "enemy/CHollowCylinder.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"

// Sets default values for this component's properties
UCGiantBeetleFSM::UCGiantBeetleFSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCGiantBeetleFSM::BeginPlay()
{
	Super::BeginPlay();

	// ...

	// 소유 객체
	Me = Cast<ACGiantBeetle>(GetOwner());

	// 플레이어
	Player1 = Cast<ACPlayer>(UGameplayStatics::GetPlayerCharacter(GetWorld(),0));
	Player2 = Cast<ACPlayer>(UGameplayStatics::GetPlayerCharacter(GetWorld(),0));
}


// Called every frame
void UCGiantBeetleFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 상태 확인용
	FString logMsg = UEnum::GetValueAsString(mState);
	GEngine->AddOnScreenDebugMessage(0, 1, FColor::Red, logMsg);

	switch (mState)
	{
		case EBeetleState::Idle:			{ IdleState();	} break;
		case EBeetleState::Retarget:		{ RetargetState();	} break;
		case EBeetleState::Charge:			{ ChargeState();	} break;
		case EBeetleState::JumpToTarget:	{ JumpToTargetState();	} break;
		case EBeetleState::TripleJump:		{ TripleJumpState();	} break;
		case EBeetleState::Damaged:			{ DamagedState();	} break;
		case EBeetleState::Die:				{ DieState();	} break;
	}
}

void UCGiantBeetleFSM::IdleState()
{
	CurIdleTime += GetWorld()->DeltaTimeSeconds;

	if (CurIdleTime > IdleDelayTime)
	{
		mState = EBeetleState::Retarget;
		CurIdleTime = 0.0f;
	}
}

void UCGiantBeetleFSM::RetargetState()
{
	Target = Target == Player1 ? Player2 : Player1;
	TargetLoc = Target->GetActorLocation() + Me->GetActorForwardVector() * 100;
	FRotator rot = UKismetMathLibrary::FindLookAtRotation(Me->GetActorLocation(),Target->GetActorLocation());
	rot.Pitch = 0;
	CurRotTime += GetWorld()->DeltaTimeSeconds;
	FRotator newRot = FMath::Lerp(Me->GetActorRotation(), rot, CurRotTime/RotTime);
	Me->SetActorRotation(newRot);
	
	if (CurRotTime >= RotTime)
	{
		if (Me->CurHP == Me->MaxHP)
		{
			mState = EBeetleState::Charge;
			//mState = EBeetleState::JumpToTarget;

		}
		else
		{
			if (ChargeCnt < MaxChargeCnt)
			{
				mState = EBeetleState::Charge;
			}
			else
			{
					JumpStartloc = Me->GetActorLocation();
					JumpEndloc = Target->GetActorLocation();
					float dist = FVector::Dist(JumpStartloc, JumpEndloc);
				if (dist > JumpMinDistance && bWasTriple)
				{
					FVector jumpDir = JumpEndloc - JumpStartloc;
					jumpDir.Z = 0;
					JumpVelocity = jumpDir.GetSafeNormal() * JumpSpeed;
					JumpTotTime = jumpDir.Length() / JumpSpeed;
					JumpVelocityZ = JumpTotTime * JumpGravity / 2;
					mState = EBeetleState::JumpToTarget;
				}
				else
				{
					mState = EBeetleState::TripleJump;
				}
			}
		}
		CurRotTime = 0.0f;
	}
}

void UCGiantBeetleFSM::ChargeState()
{
	// 타겟이 죽어있으면 Retarget

	// 돌진
	FVector curTargetLoc = Target->GetActorLocation();
	FVector curPos = Me->GetActorLocation();
	curTargetLoc.Z = curPos.Z;

	
	Me->AttackBox->SetVisibility(true);
	curPos = FMath::Lerp(curPos, TargetLoc, 1 * GetWorld()->DeltaTimeSeconds);
	Me->SetActorLocation(curPos);
	
	// 위치에 도달하면
	float dist = FVector::Distance(curPos, TargetLoc);
	if (dist < 50)
	{
		// 박치기를 실패하면 스톰프
		if (!Me->bKill) Stomp();
		mState = EBeetleState::Retarget;
		ChargeCnt++;
		Me->AttackBox->SetVisibility(false);
	}
}

void UCGiantBeetleFSM::JumpToTargetState()
{
	JumpCurTime += GetWorld()->DeltaTimeSeconds;

	float t = JumpCurTime /JumpTotTime;
	if (t >= 1)
	{
		Me->SetActorLocation(TargetLoc);
		Stomp();
		bWasTriple = false;
		ChargeCnt = 0;
		mState = EBeetleState::Idle;
		JumpCurTime = 0;
	}
	else
	{
		FVector newLoc = JumpStartloc;
		newLoc += JumpVelocity * JumpCurTime;
		newLoc.Z +=  JumpVelocityZ * JumpCurTime - 0.5 * JumpGravity * JumpCurTime * JumpCurTime;
		Me->SetActorLocation(newLoc);
	}
}

void UCGiantBeetleFSM::TripleJumpState()
{
	CurTJTime += GetWorld()->DeltaTimeSeconds;
	if (CurTJTime > TJTime)
	{
		Stomp();
		TripleCnt ++;
		CurTJTime = 0.0f;
	}

	if (TripleCnt == 3)
	{
		bWasTriple = true;
		TripleCnt = 0; 
		ChargeCnt = 0;
		mState = EBeetleState::Idle;
	}
}

void UCGiantBeetleFSM::DamagedState()
{
	if (Me->CurHP <= 0)
	{
		mState = EBeetleState::Die;
	}
	ChargeCnt = 0;
}

void UCGiantBeetleFSM::DieState()
{

}

void UCGiantBeetleFSM::Stomp()
{
	//GetWorld()->SpawnActor<ACShockwave>(ShockwaveFac, Me->GetActorTransform());
	FVector spawnLoc = Me->GetActorLocation();
	spawnLoc.Z = 0;
	GetWorld()->SpawnActor<ACHollowCylinder>(ShockCylFac, spawnLoc, FRotator());
}

