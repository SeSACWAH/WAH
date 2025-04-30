// Fill out your copyright notice in the Description page of Project Settings.


#include "Sys/CHHPlayerController.h"
#include "Sys/CHHGameInstance.h"
#include "UI/CharacterSelectWidget.h"
#include "Net/UnrealNetwork.h"
#include "EngineUtils.h"

void ACHHPlayerController::BeginPlay()
{
	Super::BeginPlay();
	

	if(IsLocalController())
	{
		auto gi = Cast<UCHHGameInstance>(GetWorld()->GetGameInstance());
		ServerRPC_ChangePlayer(gi->bTypeCody);
	}
}

void ACHHPlayerController::ServerRPC_ChangePlayer_Implementation(bool bTypeCody)
{
	if(bTypeCody)
	{
		auto oldPawn = GetPawn();
		UnPossess();
		APawn* newPawn = GetWorld()->SpawnActor<APawn>(CodyFactory, oldPawn->GetActorTransform());
		Possess(newPawn);
		oldPawn->Destroy();

	}
}

//void ACHHPlayerController::ServerRPC_SelectCody_Implementation(bool bSCody)
//{
//	UE_LOG(LogTemp,Warning,TEXT("Server"));
//	bSelectCody = bSCody;
//	MultiRPC_SelectCody(bSCody);
//}
//
//void ACHHPlayerController::MultiRPC_SelectCody_Implementation(bool bSCody)
//{
//	auto gi = Cast<UCHHGameInstance>(GetWorld()->GetGameInstance());
//	
//	widget->updateCody(bSelectCody, gi->mySessionName);
//}
//
//void ACHHPlayerController::ServerRPC_SelectMay_Implementation(bool bSMay)
//{
//	auto gi = Cast<UCHHGameInstance>(GetWorld()->GetGameInstance());
//	selectName = gi->mySessionName;
//	bSelectMay = !bSelectMay;
//	UE_LOG(LogTemp, Warning, TEXT("server / bSMay : %d"), bSelectMay);
//	MultiRPC_SelectMay(bSelectMay);
//}
//
//void ACHHPlayerController::MultiRPC_SelectMay_Implementation(bool bSMay)
//{
//	UE_LOG(LogTemp, Warning, TEXT("Multi / bSMay : %d"), bSMay);
//	bSelectMay = bSMay;
//	if (IsLocalController() && widget)
//	{
//		widget->updateMay(bSMay, selectName);
//		UE_LOG(LogTemp,Warning,TEXT("Local"));
//	}
//	else
//	{
//		UE_LOG(LogTemp,Warning,TEXT("notlocal"));
//		auto pc = Cast<ACHHPlayerController>(GetWorld()->GetFirstPlayerController());
//		pc->widget->updateMay(bSMay, selectName);
//	}
//}
//
//void ACHHPlayerController::OnPossess(APawn* aPawn)
//{
//	Super::OnPossess(aPawn);
//	if (IsLocalController())
//	{
//		widget = Cast<UCharacterSelectWidget>(CreateWidget(GetWorld(), widgetFac));
//		widget->controller = this;
//		widget->AddToViewport();
//		UE_LOG(LogTemp, Warning, (TEXT("cw")));
//	}
//}
//
//void ACHHPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//
//	DOREPLIFETIME(ACHHPlayerController, bSelectCody);
//	DOREPLIFETIME(ACHHPlayerController, bSelectMay);
//	DOREPLIFETIME(ACHHPlayerController, selectName);
//
//}
