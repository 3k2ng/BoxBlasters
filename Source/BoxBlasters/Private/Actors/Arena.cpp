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

TArray<ETileType> DefaultMap()
{
	TArray<ETileType> Map;
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
	InstancedWarning = CreateDefaultSubobject<UInstancedStaticMeshComponent>("WarningComponent");
	InstancedWarning->AttachToComponent(RootComponent,
	                                      FAttachmentTransformRules{EAttachmentRule::KeepRelative, false});
	InstancedWarning->SetMobility(EComponentMobility::Static);
	InstancedWarning->SetCollisionProfileName("NoCollision");
	TileMap = DefaultMap();
	LootMap.Init(ELootType::None, GTotal);
	BoxInstanceMap.Init(-1, GTotal);
	DangerMap.Init(false, GTotal);
	ExplosionTimerMap.Init(-1.F, GTotal);
	BombTypeMap.Init(EBombType::None, GTotal);
	BombPowerMap.Init(0, GTotal);
	AirStrikeTarget.Init({}, 4);
	AirStrikeActive.Init(false, 4);
	AirStrikePower.Init(-1, 4);
}

void AArena::BeginPlay()
{
	Super::BeginPlay();
	CHECK_VALID(InstancedWall->GetStaticMesh());
	CHECK_VALID(InstancedBox->GetStaticMesh());
	CHECK_VALID(InstancedExplosion->GetStaticMesh());
	CHECK_VALID(InstancedWarning->GetStaticMesh());
	CHECK_VALID(RedUpgrade);
	CHECK_VALID(GreenUpgrade);
	CHECK_VALID(BlueUpgrade);
	CHECK_VALID(RedSpecial);
	CHECK_VALID(GreenSpecial);
	CHECK_VALID(BlueSpecial);
	GenerateTiles(TileMap, LootMap, TileGen);
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
		struct FColor
		{
			float Red, Green, Blue;
		};
		FColor BoxColor{};
		switch (BoxType)
		{
		case ETileType::Empty:
			return;
		case ETileType::Wall:
			return;
		case ETileType::Basic:
			BoxColor = {0.5F, 0.3F, 0.2F};
			break;
		case ETileType::Reinforced:
			BoxColor = {0.1F, 0.1F, 0.2F};
			break;
		case ETileType::White:
			BoxColor = {1.0F, 1.0F, 1.0F};
			break;
		case ETileType::Red:
			BoxColor = {0.9F, 0.1F, 0.2F};
			break;
		case ETileType::Green:
			BoxColor = {0.1F, 0.8F, 0.2F};
			break;
		case ETileType::Blue:
			BoxColor = {0.1F, 0.3F, 0.8F};
			break;
		default: ;
		}
		TileMap[Tile.Index()] = BoxType;
		BoxInstanceMap[Tile.Index()] = InstancedBox->AddInstance(
			FTransform{
				FRotator{0.F},
				Tile.Location(),
				FVector{0.8F}
			}
		);
		InstancedBox->SetCustomDataValue(BoxInstanceMap[Tile.Index()], 0, BoxColor.Red, true);
		InstancedBox->SetCustomDataValue(BoxInstanceMap[Tile.Index()], 1, BoxColor.Green, true);
		InstancedBox->SetCustomDataValue(BoxInstanceMap[Tile.Index()], 2, BoxColor.Blue, true);
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

void AArena::UpdateWarning()
{
	InstancedWarning->ClearInstances();
	TArray<FTile> WarningTiles;
	for (int i = 0; i < GTotal; ++i)
	{
		DangerMap[i] = false;
		if (BombTypeMap[i] != EBombType::None)
		{
			for (const FTile BombedTile : GetBombedTiles(IndexTile(i), BombPowerMap[i]))
			{
				WarningTiles.AddUnique(BombedTile);
			}
		}
	}
	for (int i = 0; i < 4; ++i)
	{
		if (AirStrikeActive[i])
		{
			for (const FTile BombedTile : GetBombedTiles(AirStrikeTarget[i], AirStrikePower[i]))
			{
				WarningTiles.AddUnique(BombedTile);
			}
		}
	}
	for (const FTile WarningTile : WarningTiles)
	{
		InstancedWarning->AddInstance(FTransform{WarningTile.Location()});
		DangerMap[WarningTile.Index()] = true;
	}
}

void AArena::ExplodeAt(const FTile Tile)
{
	if (Tile.IsValid())
	{
		const int32 i = Tile.Index();
		if (TileMap[i] != ETileType::Wall && TileMap[i] != ETileType::Reinforced)
		{
			ExplosionTimerMap[i] = 0.2F;
		}
	}
}

TArray<FTile> AArena::GetBombedTiles(const FTile BombTile, const int32 BombPower) const
{
	TArray<FTile> BombedTiles;
	if (TileMap[BombTile.Index()] == ETileType::Wall) return BombedTiles;
	bool Blocked[] = {false, false, false, false};
	constexpr FTile ToAdd[] = {
		{0, -1}, {0, 1}, {-1, 0}, {1, 0}
	};
	BombedTiles.Add(BombTile);
	if (TileMap[BombTile.Index()] == ETileType::Reinforced) return BombedTiles;
	for (int i = 1; i <= BombPower; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			if (!Blocked[j])
			{
				FTile BombedTile = BombTile + i * ToAdd[j];
				if (BombedTile.IsValid())
				{
					Blocked[j] |= TileMap[BombedTile.Index()] == ETileType::Wall;
					Blocked[j] |= TileMap[BombedTile.Index()] == ETileType::Reinforced;
					if (TileMap[BombedTile.Index()] != ETileType::Wall) BombedTiles.Add(BombedTile);
				}
			}
		}
	}
	return BombedTiles;
}

void AArena::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
	InstancedExplosion->ClearInstances();
	for (int i = 0; i < GTotal; ++i)
	{
		if (ExplosionTimerMap[i] > 0.F)
		{
			InstancedExplosion->AddInstance(
				FTransform{
					FRotator{},
					IndexTile(i).Location(),
					FVector{1.6F}
				}
			);
			ExplosionTimerMap[i] -= DeltaTime;
		}
	}
	UpdateWarning();
}

bool AArena::PlaceBomb(const EBombType BombType, const FTile BombTile, const int32 BombPower)
{
	if (!BombTile.IsValid()) return false;
	if (BombType == EBombType::None) return false;
	if (BombTypeMap[BombTile.Index()] != EBombType::None) return false;
	BombTypeMap[BombTile.Index()] = BombType;
	BombPowerMap[BombTile.Index()] = BombPower;
	return true;
}

bool AArena::TryBreak(const FTile Tile)
{
	if (!Tile.IsValid()) return false;
	switch (TileMap[Tile.Index()])
	{
	case ETileType::Empty:
		return true;
	case ETileType::Wall:
		return false;
	case ETileType::Basic:
		TileMap[Tile.Index()] = ETileType::Empty;
		return true;
	case ETileType::Reinforced:
		TileMap[Tile.Index()] = ETileType::Basic;
		return false;
	case ETileType::White:
		TileMap[Tile.Index()] = ETileType::Empty;
		if (LootMap[Tile.Index()] == ELootType::Upgrade)
		{
			switch (FMath::RandRange(0, 2))
			{
			case 0: GetWorld()->SpawnActor<APickUp>(
					RedUpgrade,
					FTransform{Tile.Location()}
				);
				break;
			case 1: GetWorld()->SpawnActor<APickUp>(
					GreenUpgrade,
					FTransform{Tile.Location()}
				);
				break;
			case 2: GetWorld()->SpawnActor<APickUp>(
					BlueUpgrade,
					FTransform{Tile.Location()}
				);
				break;
			default: ;
			}
		}
		return true;
	case ETileType::Red:
		TileMap[Tile.Index()] = ETileType::Empty;
		if (LootMap[Tile.Index()] == ELootType::Special)
		{
			GetWorld()->SpawnActor<APickUp>(
				RedSpecial,
				FTransform{Tile.Location()}
			);
		}
		else if (LootMap[Tile.Index()] == ELootType::Upgrade)
		{
			GetWorld()->SpawnActor<APickUp>(
				RedUpgrade,
				FTransform{Tile.Location()}
			);
		}
		return true;
	case ETileType::Green:
		TileMap[Tile.Index()] = ETileType::Empty;
		if (LootMap[Tile.Index()] == ELootType::Special)
		{
			GetWorld()->SpawnActor<APickUp>(
				GreenSpecial,
				FTransform{Tile.Location()}
			);
		}
		else if (LootMap[Tile.Index()] == ELootType::Upgrade)
		{
			GetWorld()->SpawnActor<APickUp>(
				GreenUpgrade,
				FTransform{Tile.Location()}
			);
		}
		return true;
	case ETileType::Blue:
		TileMap[Tile.Index()] = ETileType::Empty;
		if (LootMap[Tile.Index()] == ELootType::Special)
		{
			GetWorld()->SpawnActor<APickUp>(
				BlueSpecial,
				FTransform{Tile.Location()}
			);
		}
		else if (LootMap[Tile.Index()] == ELootType::Upgrade)
		{
			GetWorld()->SpawnActor<APickUp>(
				BlueUpgrade,
				FTransform{Tile.Location()}
			);
		}
		return true;
	default:
		return false;
	}
}

TArray<FTile> AArena::DetonateBomb(const FTile BombTile)
{
	TArray<FTile> DestroyedTiles;
	if (!BombTile.IsValid() || BombTypeMap[BombTile.Index()] == EBombType::None) return DestroyedTiles;
	BombTypeMap[BombTile.Index()] = EBombType::None;
	BombExplode(BombTile, BombPowerMap[BombTile.Index()]);
	BombPowerMap[BombTile.Index()] = 0;
	return DestroyedTiles;
}

TArray<FTile> AArena::DetonateAirStrike(const int32 Index, const int32 BombPower)
{
	return BombExplode(AirStrikeTarget[Index], BombPower);
}

TArray<FTile> AArena::BombExplode(const FTile BombTile, const int32 BombPower)
{
	TArray<FTile> DestroyedTiles;
	if (!BombTile.IsValid()) return DestroyedTiles;
	for (const auto BombedTile : GetBombedTiles(BombTile, BombPower))
	{
		if (TryBreak(BombedTile))
		{
			DestroyedTiles.AddUnique(BombedTile);
			ExplodeAt(BombedTile);
		}
		for (const auto MoreBombedTile : DetonateBomb(BombedTile))
		{
			DestroyedTiles.AddUnique(MoreBombedTile);
		}
	}
	UpdateBoxes();
	return DestroyedTiles;
}
