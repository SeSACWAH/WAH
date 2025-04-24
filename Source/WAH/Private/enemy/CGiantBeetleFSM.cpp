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
#include "Net/UnrealNetwork.h"
#include "EngineUtils.h"



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
	//Player2 = Cast<ACPlayer>(UGameplayStatics::GetPlayerCharacter(GetWorld(),1));
	Player2 = Player1;
	//ai = Cast<AAIController>(Me->GetController());
}


// Called every frame
void UCGiantBeetleFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 상태 확인용
	FString logMsg = UEnum::GetValueAsString(mState);
	GEngine->AddOnScreenDebugMessage(0, 1, FColor::Red, logMsg);
	if (!Me->HasAuthority()) return;
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
		//Target = Target == Player1 ? Player2 : Player1;
		if (!Target && Me->HasAuthority())
		{
			TArray<AActor*> actors;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter::StaticClass(), actors);
			Target = Cast<ACPlayer>(actors[FMath::RandRange(0, actors.Num() - 1)]);
		}
		mState = EBeetleState::Retarget;
		CurIdleTime = 0.0f;
		CurRotTime = 0.0f;
	}
}

void UCGiantBeetleFSM::RetargetState()
{
	/*if(!Target && Me->HasAuthority())
	{
		TArray<AActor*> actors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter::StaticClass(), actors);
		Target = Cast<ACPlayer>(actors[FMath::RandRange(0, actors.Num()-1)]);
	}*/
	//if(Target->GetIsDead()) Target = Target == Player1 ? Player2 : Player1;
	if(!Me->HasAuthority()) return;
	TargetLoc = Target->GetActorLocation();
	FRotator rot = UKismetMathLibrary::FindLookAtRotation(Me->GetActorLocation(),Target->GetActorLocation());
	//rot.Pitch = 0;
	//rot.Roll = 0;

	FRotator r = FRotator(Me->GetActorRotation().Pitch, rot.Yaw, Me->GetActorRotation().Roll);

	CurRotTime += GetWorld()->DeltaTimeSeconds;
	float ratio = CurRotTime / RotTime;
	if( ratio > 1.0f )	ratio = 1.0f;
	
	FRotator newRot = FMath::Lerp(Me->GetActorRotation(), r, ratio);
	Me->SetActorRotation(newRot);
	//Me->SetActorRotation(rot);
	
	

	if (CurRotTime >= RotTime)
	{
		if (Me->CurHP == Me->MaxHP)
		{
			//JumpStartloc = Me->GetActorLocation();
			//JumpEndloc = Target->GetActorLocation();
			//FVector jumpDir = JumpEndloc - JumpStartloc;
			//jumpDir.Z = 0;
			//JumpVelocity = jumpDir.GetSafeNormal() * JumpSpeed;
			//JumpTotTime = jumpDir.Length() / JumpSpeed;
			//JumpVelocityZ = JumpTotTime * JumpGravity / 2;
			//mState = EBeetleState::TripleJump;
			mState = EBeetleState::Charge;
			CurRotTime = 0.0f;
			return;
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
	if(Target->GetIsDead()) 
	{
		mState = EBeetleState::Idle;
		Target = nullptr;
		return;
	}
	
	// 돌진
	FVector curTargetLoc = Target->GetActorLocation();
	FVector curPos = Me->GetActorLocation();
	curTargetLoc.Z = curPos.Z;
	FVector vel = Me->GetActorForwardVector();
	float dotRes = FVector::DotProduct(vel, curTargetLoc-curPos);
	FVector crossRes = FVector::CrossProduct( curTargetLoc - curPos , vel);
	float checkRight = FVector::DotProduct(Me->GetActorUpVector(),crossRes);
	// 어텍박스 활성화
	Me->AttackBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	
	//// lerp 직선이동
	//curPos = FMath::Lerp(curPos, TargetLoc, 1 * GetWorld()->DeltaTimeSeconds);
	//Me->SetActorLocation(curPos);

	// 약간 커브이동
	//FRotator newRot = Me->GetActorRotation();

	/*if (dotRes >= 0)
	{
		if ( crossRes.Z >10)
		{
			newRot -= FRotator(0 , 1, 0) * ChargeCurve;
		}
		else if (crossRes.Z < -10)
		{
			newRot += FRotator(0, 1, 0) * ChargeCurve;
		}
	}
	else
	{
		if (crossRes.Z > 10)
		{
			newRot += Me->GetActorRightVector().Rotation() * ChargeCurve;
		}
		else if (crossRes.Z < -10)
		{
			newRot -= Me->GetActorRightVector().Rotation() * ChargeCurve;
		}
	}*/
	//Me->SetActorRelativeRotation(newRot);
	Me->SetActorLocation(curPos + Me->GetActorForwardVector() * ChargeSpeed * GetWorld()->DeltaTimeSeconds);

	// 위치에 도달하면
	if (dotRes < -10)
	{
		// 박치기를 실패하면 스톰프
		if (!Me->bKill) Stomp();
		mState = EBeetleState::Idle;
		Target = nullptr;
		ChargeCnt++;
		Me->AttackBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Me->bKill = false;
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
		Target = nullptr;
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
		Target = nullptr;
	}
}

void UCGiantBeetleFSM::DamagedState()
{
	
	Me->SetActorLocation(Me->GetActorLocation());
	CurDMGTime += GetWorld()->DeltaTimeSeconds;
	
	if (CurDMGTime > DamageDelayTime)
	{
		mState = EBeetleState::Idle;
		Target = nullptr;

		CurDMGTime = 0.0f;

		//Anim->AnimState = mState;
		ChargeCnt = 0;
	}
}

void UCGiantBeetleFSM::DieState()
{

}

void UCGiantBeetleFSM::Stomp()
{
	//GetWorld()->SpawnActor<ACShockwave>(ShockwaveFac, Me->GetActorTransform());
	//FVector spawnLoc = Me->GetActorLocation();
	//spawnLoc.Z = 0;
	//GetWorld()->SpawnActor<ACHollowCylinder>(ShockCylFac, spawnLoc, FRotator());
	ServerRPC_Stomp();
}

void UCGiantBeetleFSM::OnDamageProcess(int32 damage)
{
	ServerRPC_OnDamage(damage);
}

void UCGiantBeetleFSM::ServerRPC_Stomp_Implementation()
{
	MultiRPC_Stomp();
}

void UCGiantBeetleFSM::MultiRPC_Stomp_Implementation()
{
	FVector spawnLoc = Me->GetActorLocation();
	spawnLoc.Z = 0;
	GetWorld()->SpawnActor<ACHollowCylinder>(ShockCylFac, spawnLoc, FRotator());
}

void UCGiantBeetleFSM::ServerRPC_OnDamage_Implementation(int32 damage)
{
	Me->CurHP -= damage;

	if (Me->CurHP > 0)
	{
		mState = EBeetleState::Damaged;

	}
	else
	{
		mState = EBeetleState::Die;
	}
}

void UCGiantBeetleFSM::MultiRPC_ONDamage_Implementation(int32 damage)
{
	
}

void UCGiantBeetleFSM::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCGiantBeetleFSM, Target);
	DOREPLIFETIME(UCGiantBeetleFSM, MaxChargeCnt);
	DOREPLIFETIME(UCGiantBeetleFSM, HP);

}

