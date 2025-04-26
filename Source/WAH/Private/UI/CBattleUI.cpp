#include "UI/CBattleUI.h"
#include "../../../../../../../Source/Runtime/Engine/Public/Materials/MaterialParameterCollectionInstance.h"

void UCBattleUI::UpdateMPCPlayerHP(bool bIsCody, float InCurHP, float InMaxHP)
{
    if (MPC == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("MaterialParameterCollection is not assigned!"));
        return;
    }

    // ���� ���忡�� MPC�� ������
    UWorld* world = GetWorld();
    if(world == nullptr) return;

    UMaterialParameterCollectionInstance* collectionInstance = world->GetParameterCollectionInstance(MPC);

    if (collectionInstance == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to get MaterialParameterCollectionInstance!"));
        return;
    }

    // HP�� 0~1 ���� ������ ����ȭ
    float normalizedHP = InCurHP / InMaxHP;

    // Scalar Parameter ������Ʈ
    FName paramName = bIsCody ? TEXT("Cody") : TEXT("May");
    collectionInstance->SetScalarParameterValue(paramName, normalizedHP);
}

