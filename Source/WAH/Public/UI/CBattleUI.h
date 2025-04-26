#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CBattleUI.generated.h"

UCLASS()
class WAH_API UCBattleUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HP")
	float HPBeetle = 1.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	class UMaterialParameterCollection* MPC;

	void UpdateMPCPlayerHP(bool bIsCody, float InCurHP, float InMaxHP);
};
