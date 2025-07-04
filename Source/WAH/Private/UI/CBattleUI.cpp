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
    UE_LOG(LogTemp, Warning, TEXT("UPDATE HP >>> MPC"));
    

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
