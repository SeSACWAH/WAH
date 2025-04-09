// Fill out your copyright notice in the Description page of Project Settings.


#include "enemy/CHollowCylinder.h"
#include "../../../../Plugins/Runtime/ProceduralMeshComponent/Source/ProceduralMeshComponent/Public/ProceduralMeshComponent.h"



// Sets default values
ACHollowCylinder::ACHollowCylinder()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProceduralMeshComp = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMeshComp"));
	SetRootComponent(ProceduralMeshComp);

}

// Called when the game starts or when spawned
void ACHollowCylinder::BeginPlay()
{
	Super::BeginPlay();
	GenerateCylinder();
}

// Called every frame
void ACHollowCylinder::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACHollowCylinder::GenerateCylinder()
{
    TArray<FVector> Vertices;
    TArray<int32> Triangles;

    // ���� ����
    for (int32 i = 0; i <= Segments; i++)
    {
        float theta = (float)i * 2.0f * PI / Segments;

        // �ܺ� ��
        float outerX = OuterRadius * FMath::Cos(theta);
        float outerY = OuterRadius * FMath::Sin(theta);
        Vertices.Add(FVector(outerX, outerY, 0.0f));        // �ϴ�
        Vertices.Add(FVector(outerX, outerY, Height));      // ���

        // ���� ��
        float innerX = InnerRadius * FMath::Cos(theta);
        float innerY = InnerRadius * FMath::Sin(theta);
        Vertices.Add(FVector(innerX, innerY, 0.0f));        // �ϴ�
        Vertices.Add(FVector(innerX, innerY, Height));      // ���
    }

    // �ﰢ�� ����
    for (int32 i = 0; i < Segments; i++)
    {
        int32 a = i * 4;
        int32 b = i * 4 + 1;
        int32 c = (i + 1) * 4;
        int32 d = (i + 1) * 4 + 1;
        int32 e = i * 4 + 2;
        int32 f = i * 4 + 3;
        int32 g = (i + 1) * 4 + 2;
        int32 h = (i + 1) * 4 + 3;

        // �ܺ� ��
        Triangles.Add(a); Triangles.Add(b); Triangles.Add(c);
        Triangles.Add(b); Triangles.Add(d); Triangles.Add(c);

        // ���� ��
        Triangles.Add(f); Triangles.Add(e); Triangles.Add(g);
        Triangles.Add(f); Triangles.Add(g); Triangles.Add(h);

        // ��� ��
        Triangles.Add(b); Triangles.Add(f); Triangles.Add(d);
        Triangles.Add(f); Triangles.Add(h); Triangles.Add(d);

        // �ϴ� ��
        Triangles.Add(e); Triangles.Add(a); Triangles.Add(c);
        Triangles.Add(e); Triangles.Add(c); Triangles.Add(g);
    }

    ProceduralMeshComp->CreateMeshSection(0, Vertices, Triangles, TArray<FVector>(), TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), true);
}

