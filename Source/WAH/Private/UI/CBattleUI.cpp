#include "UI/CBattleUI.h"
#include "../../../../../../../Source/Runtime/Engine/Public/Materials/MaterialParameterCollectionInstance.h"

void UCBattleUI::UpdateMPCPlayerHP(bool bIsCody, float InCurHP, float InMaxHP)
{
    if (MPC == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("MaterialParameterCollection is not assigned!"));
        return;
    }

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
}

