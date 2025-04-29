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
	// 원본 머티리얼 인스턴스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProgressBar|Cody")
	class UMaterialInterface* ProgressMaterialCody;  
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProgressBar|May")
	class UMaterialInterface* ProgressMaterialMay;  

	// 복사본
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ProgressBar|Cody")
	class UMaterialInstanceDynamic* DynamicMaterialCody; 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ProgressBar|May")
	class UMaterialInstanceDynamic* DynamicMaterialMay; 


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HP")
	float HPBeetle = 1.0;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Material")
	class UMaterialParameterCollection* MPC;

	void UpdateMPCPlayerHP(bool bIsCody, float InCurHP, float InMaxHP);

	//UFUNCTION(Server, Reliable)
	//void ServerRPC_UpdateMPCPlayerHP(class UMaterialParameterCollection* InMPC, bool bIsCody, float InCurHP, float InMaxHP);

	//UFUNCTION(NetMulticast, Reliable)
	//void MulticastRPC_UpdateMPCPlayerHP(class UMaterialParameterCollection* InMPC, bool bIsCody, float InCurHP, float InMaxHP);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
};
