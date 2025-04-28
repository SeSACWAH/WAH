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
#include "Animation/CbeetleAnim.h"
#include "AIController.h"



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

	// ���� ��ü
	Me = Cast<ACGiantBeetle>(GetOwner());

	// �÷��̾�
	Player1 = Cast<ACPlayer>(UGameplayStatics::GetPlayerCharacter(GetWorld(),0));
	//Player2 = Cast<ACPlayer>(UGameplayStatics::GetPlayerCharacter(GetWorld(),1));
	Player2 = Player1;
	ai = Cast<AAIController>(Me->GetController());
	Anim = Cast<UCBeetleAnim>(Me->MeshComp->GetAnimInstance());
}


// Called every frame
void UCGiantBeetleFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ���� Ȯ�ο�
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
			if(actors[0])
			{
			Target = Cast<ACPlayer>(actors[FMath::RandRange(0, actors.Num() - 1)]);
			}
		}
		mState = EBeetleState::Retarget;
		Anim->AnimState = mState;
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
			Anim->AnimState = mState;
			CurRotTime = 0.0f;
			return;
		}
		else
		{
			if (!bWasDamaged && ChargeCnt < MaxChargeCnt)
			{
				mState = EBeetleState::Charge;
				Anim->AnimState = mState;
			}
			else
			{
					JumpStartloc = Me->GetActorLocation();
					JumpEndloc = Target->GetActorLocation();
					float dist = FVector::Dist(JumpStartloc, JumpEndloc);
				if (dist > JumpMinDistance && bWasTriple)
				{
					FVector jumpDir = JumpEndloc - JumpStartloc;
					jumpDir.Z = 150;
					JumpVelocity = jumpDir.GetSafeNormal() * JumpSpeed;
					JumpTotTime = jumpDir.Length() / JumpSpeed;
					JumpVelocityZ = JumpTotTime * JumpGravity / 2;
					mState = EBeetleState::JumpToTarget;
					Anim->AnimState = mState;
				}
				else
				{
					mState = EBeetleState::TripleJump;
					Anim->AnimState = mState;
				}
				bWasDamaged = false;
			}
		}
		CurRotTime = 0.0f;
	}
}

void UCGiantBeetleFSM::ChargeState()
{
	// Ÿ���� �׾������� Retarget
	if(Target->GetIsDead()) 
	{
		mState = EBeetleState::Idle;
		Anim->AnimState = mState;
		Target = nullptr;
		return;
	}
	
	// ����
	FVector curTargetLoc = Target->GetActorLocation();
	FVector curPos = Me->GetActorLocation();
	curTargetLoc.Z = curPos.Z;
	FVector vel = Me->GetActorForwardVector();
	FVector TargetVel = curTargetLoc - curPos;
	float dotRes = FVector::DotProduct(vel, TargetVel) / TargetVel.Size();
	FVector crossRes = FVector::CrossProduct( curTargetLoc - curPos , vel);
	float checkRight = FVector::DotProduct(Me->GetActorUpVector(),crossRes);
	// ���عڽ� Ȱ��ȭ
	Me->AttackBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	
	//// lerp �����̵�
	//curPos = FMath::Lerp(curPos, TargetLoc, 1 * GetWorld()->DeltaTimeSeconds);
	//Me->SetActorLocation(curPos);

	// �ణ Ŀ���̵�
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
	UE_LOG(LogTemp,Warning,TEXT("%f"),dotRes);
	// ��ġ�� �����ϸ�
	if (dotRes < -0.5)
	{
		Me->SetActorLocation(curPos);
		Anim->bChargeEnd = true;
		// ��ġ�⸦ �����ϸ� ������
		ChargeCurtime += GetWorld()->DeltaTimeSeconds;
		if (!Me->bKill) 
		{
			Anim->IsKill = false;
			//4.3
			if(ChargeCurtime >= ChargeStompTime)
			{
				mState = EBeetleState::Idle;
				Anim->AnimState = mState;
				Target = nullptr;
				ChargeCnt++;
				Me->AttackBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				Me->bKill = false;
				Anim->IsKill = false;
				Anim->bChargeEnd = false;
				ChargeCurtime = 0.0;
			}
		}
		else 
		{
			Anim->IsKill = true;
			// 1.22
			if(ChargeCurtime >= ChargeAttackTime)
			{
				mState = EBeetleState::Idle;
				Anim->AnimState = mState;
				Target = nullptr;
				ChargeCnt++;
				Me->AttackBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				Me->bKill = false;
				Anim->IsKill = false;
				Anim->bChargeEnd = false;
				ChargeCurtime = 0.0;
			}
		}
		
	}
}

void UCGiantBeetleFSM::JumpToTargetState()
{
	JumpAgroCurTime += GetWorld()->DeltaTimeSeconds;
	if(JumpAgroCurTime > JumpAgroTime)
	{
		JumpCurTime += GetWorld()->DeltaTimeSeconds;

		float t = JumpCurTime /JumpTotTime;
		if (t >= 1)
		{
			Anim->bJumpEnd = true;
			JumpCurLandTime += GetWorld()->DeltaTimeSeconds;
			Me->SetActorLocation(TargetLoc);
			//Stomp();
			if(JumpCurLandTime >= JumpLandTime)
			{
				bWasTriple = false;
				ChargeCnt = 0;
				mState = EBeetleState::Idle;
				Anim->AnimState = mState;
				Target = nullptr;
				JumpCurTime = 0;
				JumpAgroCurTime = 0;
				Anim->bJumpEnd = false;
				JumpCurLandTime = 0;
			}
		}
		else
		{
			FVector newLoc = JumpStartloc;
			newLoc += JumpVelocity * JumpCurTime;
			newLoc.Z +=  JumpVelocityZ * JumpCurTime - 0.5 * JumpGravity * JumpCurTime * JumpCurTime + 150;
			Me->SetActorLocation(newLoc);
		}
	}
}

void UCGiantBeetleFSM::TripleJumpState()
{
	TJumpAgroCurTime += GetWorld()->GetDeltaSeconds();
	if (TJumpAgroCurTime > TJumpAgroTime)
	{
		CurTJTime += GetWorld()->DeltaTimeSeconds;
		if (CurTJTime > TJTime)
		{
			//Stomp();
			TripleCnt++;
			CurTJTime = 0.0f;
		}

		if (TripleCnt == 3)
		{
			bWasTriple = true;
			TripleCnt = 0;
			ChargeCnt = 0;
			mState = EBeetleState::Idle;
			TJumpAgroCurTime = 0;
			Anim->AnimState = mState;
			Target = nullptr;
		}
	}
}

void UCGiantBeetleFSM::DamagedState()
{
	CurDMGTime += GetWorld()->DeltaTimeSeconds;
	
	if (CurDMGTime > DamageDelayTime)
	{
		mState = EBeetleState::Idle;
		Anim->AnimState = mState;
		Target = nullptr;

		CurDMGTime = 0.0f;

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
	Me->AttackBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	bWasDamaged = true;
	if (Me->CurHP > 0)
	{
		Anim->PlayDamagedMontage();
		mState = EBeetleState::Damaged;
	}
	else
	{
		Anim->PlayDieMontage();
		mState = EBeetleState::Die;
	}
	ai->StopMovement();
	Anim->AnimState = mState;
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

