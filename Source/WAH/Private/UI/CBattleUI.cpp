#include "UI/CBattleUI.h"
#include "../../../../../../../Source/Runtime/Engine/Public/Materials/MaterialParameterCollectionInstance.h"
#include "Components/Image.h"
#include "Blueprint/WidgetTree.h"
#include "../../../../../../../Source/Runtime/Engine/Public/Net/UnrealNetwork.h"

void UCBattleUI::NativeConstruct()
{
    Super::NativeConstruct();

    // Cody
    if (ProgressMaterialCody) 
        DynamicMaterialCody = UMaterialInstanceDynamic::Create(ProgressMaterialCody, this);

    if (UImage* progressImageCody = Cast<UImage>(WidgetTree->FindWidget(TEXT("ProgressBar_Cody"))))
        progressImageCody->SetBrushFromMaterial(DynamicMaterialCody);

    // May
    if (ProgressMaterialMay)
        DynamicMaterialMay = UMaterialInstanceDynamic::Create(ProgressMaterialMay, this);

    if (UImage* progressImageMay = Cast<UImage>(WidgetTree->FindWidget(TEXT("ProgressBar_May"))))
        progressImageMay->SetBrushFromMaterial(DynamicMaterialMay);
}

void UCBattleUI::UpdateMPCPlayerHP(bool bIsCody, float InCurHP, float InMaxHP)
{
    UE_LOG(LogTemp, Warning, TEXT("UPDATE HP >>> Client"));
    //ServerRPC_UpdateMPCPlayerHP(MPC, bIsCody, InCurHP, InMaxHP);
    UE_LOG(LogTemp, Warning, TEXT("UPDATE HP >>> MPC"));
    
    //if (MPC == nullptr)
    //{
    //    UE_LOG(LogTemp, Warning, TEXT("MaterialParameterCollection is not assigned!"));
    //    return;
    //}

    // 현재 월드에서 MPC를 가져옴
    UWorld* world = GetWorld();
    if(world == nullptr) return;

    UMaterialParameterCollectionInstance* collectionInstance = world->GetParameterCollectionInstance(MPC);

    if (collectionInstance == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to get MaterialParameterCollectionInstance!"));
        return;
    }

    // HP를 0~1 사이 값으로 정규화
    float normalizedHP = InCurHP / InMaxHP;

    // Scalar Parameter 업데이트
    FName paramName = bIsCody ? TEXT("Cody") : TEXT("May");
    collectionInstance->SetScalarParameterValue(paramName, normalizedHP);

    float outValue;
    collectionInstance->GetScalarParameterValue(paramName, outValue);
    UE_LOG(LogTemp, Warning, TEXT("UPDATE HP >>> Updated %s's HP : %f"), *paramName.ToString(), outValue);
}

//void UCBattleUI::ServerRPC_UpdateMPCPlayerHP_Implementation(class UMaterialParameterCollection* InMPC, bool bIsCody, float InCurHP, float InMaxHP)
//{
//    //UE_LOG(LogTemp, Warning, TEXT("UPDATE HP >>> ServerRPC"));
//    MulticastRPC_UpdateMPCPlayerHP(InMPC, bIsCody, InCurHP, InMaxHP);
//}
//
//void UCBattleUI::MulticastRPC_UpdateMPCPlayerHP_Implementation(class UMaterialParameterCollection* InMPC, bool bIsCody, float InCurHP, float InMaxHP)
//{
//    UE_LOG(LogTemp, Warning, TEXT("UPDATE HP >>> MulticastRPC"));
//    //UE_LOG(LogTemp, Warning, TEXT("UPDATE HP >>> MPC"));
//    if (InMPC == nullptr)
//    {
//        UE_LOG(LogTemp, Warning, TEXT("MaterialParameterCollection is not assigned!"));
//        return;
//    }
//
//    // 현재 월드에서 MPC를 가져옴
//    UWorld* world = GetWorld();
//    if (world == nullptr) return;
//
//    UMaterialParameterCollectionInstance* collectionInstance = world->GetParameterCollectionInstance(InMPC);
//
//    if (collectionInstance == nullptr)
//    {
//        UE_LOG(LogTemp, Warning, TEXT("Failed to get MaterialParameterCollectionInstance!"));
//        return;
//    }
//
//    // HP를 0~1 사이 값으로 정규화
//    float normalizedHP = InCurHP / InMaxHP;
//
//    // Scalar Parameter 업데이트
//    FName paramName = bIsCody ? TEXT("Cody") : TEXT("May");
//    collectionInstance->SetScalarParameterValue(paramName, normalizedHP);
//
//    float outValue;
//    collectionInstance->GetScalarParameterValue(paramName, outValue);
//    UE_LOG(LogTemp, Warning, TEXT("UPDATE HP >>> Updated %s's HP : %f"), *paramName.ToString(), outValue);
//}

void UCBattleUI::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    //DOREPLIFETIME(UCBattleUI, );
    DOREPLIFETIME(UCBattleUI, MPC);
}

