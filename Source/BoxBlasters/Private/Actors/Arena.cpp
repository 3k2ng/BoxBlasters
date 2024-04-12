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

TArray<TEnumAsByte<ETileType>> DefaultMap()
{
	TArray<TEnumAsByte<ETileType>> Map;
	Map.Init(ETileType::Empty, GTotal);
	for (int i = 0; i < GTotal; ++i)
	{
		const auto Tile = IndexTile(i);
		if (Tile.X % 2 == 1 && Tile.Y % 2 == 1) Map[i] = ETileType::Wall;
	}
	return Map;
}

AArena::AArena()
{
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent->SetMobility(EComponentMobility::Static);
	InstancedWall = CreateDefaultSubobject<UInstancedStaticMeshComponent>("WallComponent");
	InstancedWall->AttachToComponent(RootComponent, FAttachmentTransformRules{EAttachmentRule::KeepRelative, false});
	InstancedWall->SetMobility(EComponentMobility::Static);
	InstancedWall->SetCollisionProfileName("BlockAll");
	InstancedBox = CreateDefaultSubobject<UInstancedStaticMeshComponent>("BoxComponent");
	InstancedBox->AttachToComponent(RootComponent, FAttachmentTransformRules{EAttachmentRule::KeepRelative, false});
	InstancedBox->SetMobility(EComponentMobility::Static);
	InstancedBox->SetCollisionProfileName("BlockAll");
	InstancedBox->NumCustomDataFloats = 3;
	InstancedExplosion = CreateDefaultSubobject<UInstancedStaticMeshComponent>("ExplosionComponent");
	InstancedExplosion->AttachToComponent(RootComponent,
	                                      FAttachmentTransformRules{EAttachmentRule::KeepRelative, false});
	InstancedExplosion->SetMobility(EComponentMobility::Static);
	InstancedExplosion->SetCollisionProfileName("NoCollision");
	TileMap = DefaultMap();
	LootMap.Init(ELootType::None, GTotal);
	BoxInstanceMap.Init(-1, GTotal);
	AreaObjectMap.Init(nullptr, GTotal);
	AreaStateMap.Init(EAreaState::Normal, GTotal);
	ExplosionTimerMap.Init(0.F, GTotal);
	BombTypeMap.Init(EBombType::None, GTotal);
	BombPowerMap.Init(0, GTotal);
}

void AArena::BeginPlay()
{
	Super::BeginPlay();
	CHECK_VALID(InstancedWall->GetStaticMesh())
	CHECK_VALID(InstancedBox->GetStaticMesh())
	CHECK_VALID(InstancedExplosion->GetStaticMesh())
	CHECK_VALID(AreaClass)
	for (int i = 0; i < GTotal; ++i)
	{
		if (TileMap[i] != ETileType::Wall)
		{
			AreaObjectMap[i] = GetWorld()->SpawnActor<AArea>(
				AreaClass,
				FTransform{IndexTile(i).Location()}
			);
		}
	}
	UpdateBoxes();
}

void AArena::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (IsValid(InstancedWall->GetStaticMesh()))
	{
		InstancedWall->ClearInstances();
		InstancedWall->AddInstances(GetFloor(), false);
		InstancedWall->AddInstances(GetBorder(), false);
		for (int i = 0; i < GTotal; ++i)
		{
			if (TileMap[i] == ETileType::Wall) InstancedWall->AddInstance(FTransform{IndexTile(i).Location()});
		}
	}
}

void AArena::AddBox(const FTile Tile, const ETileType BoxType)
{
	if (Tile.IsValid())
	{
		float RedValue = 0.F;
		float GreenValue = 0.F;
		float BlueValue = 0.F;
		switch (BoxType)
		{
		case ETileType::Empty:
			return;
		case ETileType::Wall:
			return;
		case ETileType::Basic:
			RedValue = 0.5F;
			GreenValue = 0.5F;
			BlueValue = 0.5F;
			break;
		case ETileType::Reinforced:
			RedValue = 0.F;
			GreenValue = 0.F;
			BlueValue = 0.F;
			break;
		case ETileType::White:
			RedValue = 1.F;
			GreenValue = 1.F;
			BlueValue = 1.F;
			break;
		case ETileType::Red:
			RedValue = 1.F;
			GreenValue = 0.F;
			BlueValue = 0.F;
			break;
		case ETileType::Green:
			RedValue = 0.F;
			GreenValue = 1.F;
			BlueValue = 0.F;
			break;
		case ETileType::Blue:
			RedValue = 0.F;
			GreenValue = 0.F;
			BlueValue = 1.F;
			break;
		default: ;
		}
		TileMap[Tile.Index()] = BoxType;
		BoxInstanceMap[Tile.Index()] = InstancedBox->AddInstance(
			FTransform{
				FRotator{},
				Tile.Location(),
				{0.8F, 0.8F, 1.F}
			}
		);
		InstancedBox->SetCustomDataValue(BoxInstanceMap[Tile.Index()], 0, RedValue, true);
		InstancedBox->SetCustomDataValue(BoxInstanceMap[Tile.Index()], 1, GreenValue, true);
		InstancedBox->SetCustomDataValue(BoxInstanceMap[Tile.Index()], 2, BlueValue, true);
	}
}

void AArena::UpdateBoxes()
{
	InstancedBox->ClearInstances();
	for (int i = 0; i < GTotal; ++i)
	{
		AddBox(IndexTile(i), TileMap[i]);
	}
}

void AArena::Explode(const FTile Tile)
{
	if (Tile.IsValid())
	{
		const int32 i = Tile.Index();
		if (TileMap[i] != ETileType::Wall && TileMap[i] != ETileType::Reinforced)
		{
			ExplosionTimerMap[i] = 0.5F;
		}
	}
}

void AArena::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
	InstancedExplosion->ClearInstances();
	for (int i = 0; i < GTotal; ++i)
	{
		if (ExplosionTimerMap[i] > 0.F)
		{
			InstancedExplosion->AddInstance(FTransform{IndexTile(i).Location()});
			ExplosionTimerMap[i] -= DeltaTime;
		}
		else AreaStateMap[i] = EAreaState::Normal;
		if (IsValid(AreaObjectMap[i])) AreaObjectMap[i]->OnStateChanged(AreaStateMap[i]);
	}
}
