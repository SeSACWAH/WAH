#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CBattleUI.generated.h"

UCLASS()
class WAH_API UCBattleUI : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	// ���� ��Ƽ���� �ν��Ͻ�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProgressBar|Cody")
	class UMaterialInterface* ProgressMaterialCody;  
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProgressBar|May")
	class UMaterialInterface* ProgressMaterialMay;  

	// ���纻
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ProgressBar|Cody")
	class UMaterialInstanceDynamic* DynamicMaterialCody; 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ProgressBar|May")
	class UMaterialInstanceDynamic* DynamicMaterialMay; 


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HP")
	float HPBeetle = 1.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	class UMaterialParameterCollection* MPC;

	void UpdateMPCPlayerHP(bool bIsCody, float InCurHP, float InMaxHP);
};
