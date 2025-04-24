#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "WPlayerController.generated.h"

UENUM(BlueprintType)
enum class EPlayerRole : uint8
{
    Cody, May, MAX
};

UCLASS()
class WAH_API AWPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;
	
public:
	UPROPERTY(BlueprintReadWrite)
	class UUserWidget* ChooseUI;

public:
	UFUNCTION(BlueprintCallable)
	void SetChooseUIRef(UUserWidget* InWidget);

	UFUNCTION(Server, Reliable)
	void ServerRPC_RequestSpawn(EPlayerRole InPlayerRole);

	UFUNCTION(Client, Reliable)
	void ClientRPC_RemoveChoosePlayerUI();

    UPROPERTY(Replicated)
    EPlayerRole SelectedRole;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
};
