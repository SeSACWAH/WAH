#include "Guns/CGun.h"
#include "../../../../../../../Source/Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "../../../../../../../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraFunctionLibrary.h"
#include "../../../../../../../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h"
#include "../../../../../../../Plugins/FX/Niagara/Source/Niagara/Classes/NiagaraSystem.h"

ACGun::ACGun()
{
	PrimaryActorTick.bCanEverTick = true;

	// Gun
	GunMeshComp = CreateDefaultSubobject<UStaticMeshComponent>( TEXT("GunMeshComp") );
	GunMeshComp->SetupAttachment(RootComponent);

	ConstructorHelpers::FObjectFinder<UStaticMesh> tmpMesh(TEXT("/Script/Engine.StaticMesh'/Game/DYL/Meshes/PistolMesh/SM_Pistol.SM_Pistol'"));
	if (tmpMesh.Succeeded()) GunMeshComp->SetStaticMesh(tmpMesh.Object);

	// Fire Niagara
	//InitializeFireFXSystem();
	//InitializeFireFXPool();
}

void ACGun::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACGun::InitializeFireFXSystem()
{
	for (int32 i = 0; i < FireFXNum; i++)
	{
		ConstructorHelpers::FObjectFinder<UNiagaraSystem> tmpNiagara(TEXT(""));
		if (tmpNiagara.Succeeded()) FireFXSystems.Add(tmpNiagara.Object);
		else UE_LOG(LogTemp, Warning, TEXT("Failed to load Niagara System at index: %d"), i);

	}
}

void ACGun::InitializeFireFXPool()
{
	for (int32 i = 0; i < FireFXNum; i++)
	{
		//UNiagaraComponent* fireFXComp = NewObject<UNiagaraComponent>(this);
		UNiagaraComponent* fireFXComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("FireFXComp"));

		if (FireFXSystems[i])
		{
			fireFXComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), FireFXSystems[i], GunMeshComp->GetSocketLocation(FName("FirePos")));
			fireFXComp->RegisterComponent();
			fireFXComp->Deactivate();
			fireFXComp->OnSystemFinished.AddDynamic(this, &ACGun::OnFireFXFinished);

			FireFXPool.Add(fireFXComp);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Invalid FireFXSystem index: %d"), i);
			break;
		}
	}
}

void ACGun::PlayFireFX()
{
	for (UNiagaraComponent* fireFXComp : FireFXPool)
	{
		if (!fireFXComp->IsActive())
		{
			fireFXComp->Activate();
			break;
		}
	}
}

void ACGun::OnFireFXFinished(UNiagaraComponent* InComp)
{
	InComp->Deactivate();
}



