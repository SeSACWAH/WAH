// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/CBeetleAnim.h"
#include "enemy/CGiantBeetle.h"
#include "Net/UnrealNetwork.h"

void UCBeetleAnim::AnimNotify_Stomp()
{
	ACGiantBeetle* Beetle = Cast<ACGiantBeetle>(TryGetPawnOwner());
	if(Beetle)
	{
		Beetle->fsm->Stomp();
	}
}

void UCBeetleAnim::PlayDamagedMontage()
{
	Montage_Play(DamageAnim);
}

void UCBeetleAnim::PlayDieMontage()
{
	Montage_Play(DieAnim);
}

void UCBeetleAnim::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCBeetleAnim, AnimState);
	//DOREPLIFETIME(UCBeetleAnim, IsKill);
	//DOREPLIFETIME(UCBeetleAnim, bChargeEnd);
	//DOREPLIFETIME(UCBeetleAnim, bJumpEnd);
}