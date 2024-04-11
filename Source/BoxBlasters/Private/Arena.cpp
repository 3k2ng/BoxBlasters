#include "Arena.h"

TArray<FTransform> GetFloor()
{
	TArray<FTransform> Floor;
	for (int i = 0; i < GTotal; ++i)
	{
		Floor.Emplace(IndexTile(i).Location() + GTileSize * FVector::DownVector);
	}
	return Floor;
}

TArray<FTransform> GetBorder()
{
	TArray<FTransform> Border;
	for (int i = -1; i < GX + 1; ++i)
	{
		Border.Emplace(FTile{i, -1}.Location());
		Border.Emplace(FTile{i, GY}.Location());
	}
	for (int i = -1; i < GY + 1; ++i)
	{
		Border.Emplace(FTile{-1, i}.Location());
		Border.Emplace(FTile{GX, i}.Location());
	}
	return Border;
}

AArena::AArena()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
	BaseMeshInstance = CreateDefaultSubobject<UInstancedStaticMeshComponent>("base");
	SetRootComponent(BaseMeshInstance);
	BaseMeshInstance->SetMobility(EComponentMobility::Static);
	BaseMeshInstance->SetCollisionProfileName("BlockAll");
}

void AArena::BeginPlay()
{
	Super::BeginPlay();
	CHECK_VALID(BaseMesh);
}

void AArena::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (IsValid(BaseMesh) && !IsValid(BaseMeshInstance->GetStaticMesh()))
	{
		BaseMeshInstance->SetStaticMesh(BaseMesh);
	}
	if (IsValid(BaseMeshInstance->GetStaticMesh()))
	{
		BaseMeshInstance->ClearInstances();
		BaseMeshInstance->AddInstances(GetFloor(), false);
		BaseMeshInstance->AddInstances(GetBorder(), false);
	}
}
