#include "Actors/Arena.h"

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

TArray<FTransform> GetWalls()
{
	TArray<FTransform> Walls;
	for (int i = 0; i < GX; ++i)
	{
		for (int j = 0; j < GY; ++j)
		{
			if (i % 2 == 1 && j % 2 == 1)
			{
				Walls.Emplace(FTile{i, j}.Location());
			}
		}
	}
	return Walls;
}

AArena::AArena()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
	BaseComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>("InstancedBaseMesh");
	SetRootComponent(BaseComponent);
	BaseComponent->SetMobility(EComponentMobility::Static);
	BaseComponent->SetCollisionProfileName("BlockAll");
}

void AArena::BeginPlay()
{
	Super::BeginPlay();
	CHECK_VALID(BaseMesh);
}

void AArena::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (IsValid(BaseMesh) && !IsValid(BaseComponent->GetStaticMesh()))
	{
		BaseComponent->SetStaticMesh(BaseMesh);
	}
	if (IsValid(BaseComponent->GetStaticMesh()))
	{
		BaseComponent->ClearInstances();
		BaseComponent->AddInstances(GetFloor(), false);
		BaseComponent->AddInstances(GetBorder(), false);
		BaseComponent->AddInstances(GetWalls(), false);
	}
}
