#include "Libraries/PlannerUtils.h"

#include <queue>

FMapState::FMapState(const AArmedBomber* Bomber)
{
	CHECK_VALID(Bomber);
	CHECK_VALID(Bomber->Arena);
	const APopulatedArena* Arena = Cast<APopulatedArena>(Bomber->Arena);
	CHECK_VALID(Arena);
	TileMap = Arena->TileMap;
	BombTypeMap = Arena->BombTypeMap;
	BombPowerMap = Arena->BombPowerMap;
}

TArray<ETileState> FMapState::TileState() const
{
	TArray<ETileState> TileStateMap;
	TileStateMap.Init(ETileState::Normal, GTotal);
	for (int i = 0; i < GTotal; ++i)
	{
		if (TileMap[i] != ETileType::Empty || BombTypeMap[i] != EBombType::None)
			TileStateMap[i] = ETileState::Blocked;
		if (BombTypeMap[i] != EBombType::None)
		{
			for (const FTile BombedTile : GetBombedTiles(IndexTile(i), BombPowerMap[i]))
			{
				if (TileStateMap[BombedTile.Index()] == ETileState::Normal)
					TileStateMap[BombedTile.Index()] = ETileState::Warning;
			}
		}
	}
	return TileStateMap;
}

TArray<FTile> FMapState::GetBombedTiles(const FTile BombTile, const int32 BombPower) const
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

FMapState FMapState::DamageTiles(TArray<FTile> ToDamage) const
{
	FMapState Result = *this;
	for (const FTile DamagedTile : ToDamage)
	{
		if (TileMap[DamagedTile.Index()] == ETileType::Reinforced)
			Result.TileMap[DamagedTile.Index()] = ETileType::Basic;
		else if (TileMap[DamagedTile.Index()] != ETileType::Wall)
			Result.TileMap[DamagedTile.Index()] = ETileType::Empty;
	}
	return Result;
}

FMapState FMapState::PlaceBomb(const FTile BombTile, const EBombType BombType, const int32 BombPower) const
{
	FMapState Result = *this;
	if (Result.TileMap[BombTile.Index()] == ETileType::Empty)
	{
		Result.BombTypeMap[BombTile.Index()] = BombType;
		Result.BombPowerMap[BombTile.Index()] = BombPower;
	}
	return Result;
}

FMapState FMapState::DetonateBomb(const FTile Target) const
{
	FMapState Result = *this;
	TArray<FTile> ToDamage;
	std::queue<FTile> ToDetonate;
	ToDetonate.push(Target);
	while (!ToDetonate.empty())
	{
		FTile Current = ToDetonate.front();
		ToDetonate.pop();
		if (Result.BombTypeMap[Current.Index()] != EBombType::None)
		{
			Result.BombTypeMap[Current.Index()] = EBombType::None;
			Result.BombPowerMap[Current.Index()] = 0;
			for (const FTile BombedTile : GetBombedTiles(Current, BombPowerMap[Current.Index()]))
			{
				ToDamage.AddUnique(BombedTile);
			}
		}
	}
	return Result.DamageTiles(ToDamage);
}

FRobotPlan UPlannerUtils::PlanToReach(const AArmedBomber* Bomber, const FTile Target)
{
	FMapState InitMapState(Bomber);
	return {};
}
