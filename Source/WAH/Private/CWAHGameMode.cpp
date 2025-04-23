// Fill out your copyright notice in the Description page of Project Settings.


#include "CWAHGameMode.h"
#include "Sys/WPlayerController.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/GameFramework/PlayerStart.h"
#include "EngineUtils.h"

ACWAHGameMode::ACWAHGameMode()
{
    bStartPlayersAsSpectators = true;

    ConstructorHelpers::FClassFinder<APlayerController> controller(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/DYL/UIs/WBP_ChoosePlayer.WBP_ChoosePlayer'_C"));
    if (controller.Succeeded())
        PlayerControllerClass = controller.Class;
}

void ACWAHGameMode::SpawnPlayerCharacter(AController* Controller)
{
    if (AWPlayerController* pc = Cast<AWPlayerController>(Controller))
    {
        FName desiredTag = (pc->SelectedRole == EPlayerRole::Cody) ? "Cody" : "May";

        APlayerStart* start = nullptr;
        for (TActorIterator<APlayerStart> iter(GetWorld()); iter; ++iter)
        {
            if (iter->PlayerStartTag == desiredTag)
            {
                start = *iter;
                break;
            }
        }

        if (start)
        {
            TSubclassOf<APawn> classToSpawn = (pc->SelectedRole == EPlayerRole::Cody) ? ClassCody : ClassMay;

            if (classToSpawn)
            {
                APawn* spawnedPawn = GetWorld()->SpawnActor<APawn>(classToSpawn, start->GetActorLocation(), start->GetActorRotation());
                if(IsValid(spawnedPawn))
                    Controller->Possess(spawnedPawn);
            }

            start->Destroy();
        }
    }
}
