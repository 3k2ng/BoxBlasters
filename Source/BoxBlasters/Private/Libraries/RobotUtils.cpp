#include "Libraries/RobotUtils.h"

#include <queue>
#include <string>

#include "Actors/PopulatedArena.h"

FStatusMap::FStatusMap(const ABomber* Bomber)
{
	CHECK_VALID(Bomber);
	const APopulatedArena* Arena = Cast<APopulatedArena>(Bomber->Arena);
	CHECK_VALID(Arena);
	TileStatusMap.Init(ETileStatus::Safe, GTotal);
	BombPowerMap = Arena->BombPowerMap;
	for (int i = 0; i < GTotal; ++i)
	{
		if (Arena->TileMap[i] == ETileType::Wall) TileStatusMap[i] = ETileStatus::Wall;
		else if (Arena->TileMap[i] == ETileType::Reinforced) TileStatusMap[i] = ETileStatus::Reinforced;
		else if (Arena->TileMap[i] != ETileType::Empty) TileStatusMap[i] = ETileStatus::Breakable;
		else
		{
			if (Arena->BombTypeMap[i] != EBombType::None) TileStatusMap[i] = ETileStatus::Bomb;
			else if (Arena->DangerMap[i]) TileStatusMap[i] = ETileStatus::Danger;
			else TileStatusMap[i] = ETileStatus::Safe;
		}
	}
	for (int i = 0; i < 4; ++i)
	{
		if (i != Bomber->Index && IsValid(Arena->Bombers[i]))
		{
			BomberTile[i] = FMaybeTile::Just(Arena->Bombers[i]->CurrentTile);
			TileStatusMap[Arena->Bombers[i]->CurrentTile.Index()] = ETileStatus::Breakable;
		}
		else BomberTile[i] = FMaybeTile::None();
	}
}

void LogStatusMap(const FStatusMap& StatusMap)
{
	std::string LogOutput = "";
	for (int i = 0; i < GY; ++i)
	{
		for (int j = 0; j < GX; ++j)
		{
			switch (StatusMap.TileStatusMap[i * GX + j]) {
			case ETileStatus::Safe:
				LogOutput += ".";
				break;
			case ETileStatus::Danger:
				LogOutput += "_";
				break;
			case ETileStatus::Bomb:
				LogOutput += "6";
				break;
			case ETileStatus::Wall:
				LogOutput += "X";
				break;
			case ETileStatus::Breakable:
				LogOutput += "*";
				break;
			case ETileStatus::Reinforced:
				LogOutput += "#";
				break;
			}
		}
		LogOutput += "\n";
	}
	UE_LOG(LogTemp, Log, TEXT("%hs"), LogOutput.c_str());
}

bool FStatusMap::IsSafe(const FTile A) const
{
	return TileStatusMap[A.Index()] == ETileStatus::Safe;
}

bool FStatusMap::IsBlocked(const FTile A) const
{
	return TileStatusMap[A.Index()] != ETileStatus::Safe && TileStatusMap[A.Index()] != ETileStatus::Danger;
}

TArray<FTile> FStatusMap::ReachableTiles(const FTile A, const bool Safe) const
{
	TArray<bool> Visited;
	Visited.Init(false, GTotal);
	std::queue<FTile> ToVisit;
	ToVisit.push(A);
	TArray<FTile> Reachable;
	if (!IsBlocked(A) && (!Safe || IsSafe(A))) Reachable.Add(A);
	while (!ToVisit.empty())
	{
		FTile C = ToVisit.front();
		ToVisit.pop();
		if (Visited[C.Index()]) continue;
		Visited[C.Index()] = true;
		for (const FTile N : C.Neighbors())
		{
			if (!IsBlocked(N) && (!Safe || IsSafe(N)) && !Visited[N.Index()])
			{
				ToVisit.push(N);
				Reachable.Add(N);
			}
		}
	}
	return Reachable;
}

int32 FStatusMap::WalkCost(const FTile A, const FTile B, const int32 SafeCost, const int32 DangerCost) const
{
	TArray<bool> Visited;
	Visited.Init(false, GTotal);
	std::queue<std::pair<FTile, int32>> ToVisit;
	ToVisit.emplace(A, 0);
	while (!ToVisit.empty())
	{
		std::pair<FTile, int32> C = ToVisit.front();
		ToVisit.pop();
		if (Visited[C.first.Index()]) continue;
		Visited[C.first.Index()] = true;
		if (C.first == B) return C.second;
		for (const FTile N : C.first.Neighbors())
		{
			if (!IsBlocked(N))
			{
				ToVisit.emplace(N, C.second + (IsSafe(N) ? SafeCost : DangerCost));
			}
		}
	}
	return MAX_int32;
}

struct FAStarNode
{
	FTile Tile;
	int32 Cost;

	FAStarNode(const FTile A, const int32 InCost) : Tile(A), Cost(InCost)
	{
	}
};

inline bool operator<(const FAStarNode& A, const FAStarNode& B) { return A.Cost > B.Cost; }

TArray<FTile> FStatusMap::AStar(const FTile A, const FTile B, const int32 SafeCost, const int32 DangerCost, const int32 Weight) const
{
	TArray<FTile> Parent;
	Parent.Init({}, GTotal);
	for (int i = 0; i < GTotal; ++i)
	{
		Parent[i] = IndexTile(i);
	}
	TArray<int32> Cost;
	Cost.Init(MAX_int32, GTotal);
	std::priority_queue<FAStarNode> ToVisit;
	ToVisit.emplace(A, TileDistance(A, B) * Weight);
	while (!ToVisit.empty())
	{
		FAStarNode Current = ToVisit.top();
		ToVisit.pop();
		if (Current.Cost >= Cost[Current.Tile.Index()]) continue;
		Cost[Current.Tile.Index()] = Current.Cost;
		if (Current.Tile == B)
		{
			TArray<FTile> ReversedPath;
			FTile Trace = B;
			while (Parent[Trace.Index()] != Trace)
			{
				ReversedPath.Add(Trace);
				Trace = Parent[Trace.Index()];
			}
			TArray<FTile> Path;
			for (int i = ReversedPath.Num() - 1; i >= 0; --i)
			{
				Path.Add(ReversedPath[i]);
			}
			return Path;
		}
		for (const FTile Neighbor : Current.Tile.Neighbors())
		{
			if (!IsBlocked(Neighbor))
			{
				int32 NewCost = Current.Cost + (IsSafe(Neighbor) ? SafeCost : DangerCost) + TileDistance(Current.Tile, B) * Weight;
				if (NewCost < Cost[Neighbor.Index()])
				{
					Parent[Neighbor.Index()] = Current.Tile;
					ToVisit.emplace(Neighbor, NewCost);
				}
			}
		}
	}
	return {};
}

struct FBombAStarNode
{
	FStatusMap StatusMap;
	TArray<bool> Bombed;
	TArray<FTile> Bombs;
	int32 Cost;

	FBombAStarNode(const FStatusMap& InSM, const TArray<bool>& InBombed, const TArray<FTile>& InBombs, const int32 InCost) :
		StatusMap(InSM),
		Bombed(InBombed),
		Bombs(InBombs),
		Cost(InCost)
	{
	}

	static TArray<FBombAStarNode> Init(const FStatusMap& StatusMap, const FTile A, const FTile B, const int32 BombPower, const int32 BombCost, const int32 Weight)
	{
		TArray<FBombAStarNode> InitStates;
		for (const FTile BombTile : StatusMap.ViableBombSpots(A, BombPower))
		{
			int32 BestScore = MAX_int32;
			for (const FTile ImpactedTile : StatusMap.BombImpact(BombTile, BombPower))
			{
				const int32 CurrentScore = TileDistance(ImpactedTile, B);
				if (BestScore > CurrentScore) BestScore = CurrentScore;
			}
			TArray<bool> Bombed;
			Bombed.Init(false, GTotal);
			Bombed[BombTile.Index()] = true;
			InitStates.Emplace(
				StatusMap.PlaceBomb(BombTile, BombPower).DetonateBomb(BombTile),
				Bombed,
				TArray<FTile>{ BombTile },
				BombCost + BestScore * Weight
			);
		}
		return InitStates;
	};
};

inline bool operator<(const FBombAStarNode& A, const FBombAStarNode& B) { return A.Cost > B.Cost; }

TArray<FTile> FStatusMap::BombAStar(const FTile A, const FTile B, const int32 BombPower, const int32 BombCost, const int32 Weight) const
{
	TArray<int32> Cost[2];
	Cost[0].Init(MAX_int32, GTotal);
	Cost[1].Init(MAX_int32, GTotal);
	std::priority_queue<FBombAStarNode> ToVisit;
	for (const FBombAStarNode InitState : FBombAStarNode::Init(*this, A, B, BombPower, BombCost, Weight))
	{
		ToVisit.push(InitState);
	}
	while (!ToVisit.empty())
	{
		const FBombAStarNode Current = ToVisit.top();
		const FTile CurrentTile = Current.Bombs.Last();
		const bool CurrentIsSecond = Current.Bombed[CurrentTile.Index()];
		ToVisit.pop();
		if (Current.Cost >= Cost[CurrentIsSecond ? 1 : 0][Current.Bombs.Last().Index()]) continue;
		Cost[CurrentIsSecond ? 1 : 0][Current.Bombs.Last().Index()] = Current.Cost;
		if (Current.StatusMap.ReachableTiles(A, false).Contains(B))
		{
			return Current.Bombs;
		}
		for (const FTile BombTile : Current.StatusMap.ViableBombSpots(A, BombPower))
		{
			int32 BestScore = MAX_int32;
			for (const FTile ImpactedTile : Current.StatusMap.BombImpact(BombTile, BombPower))
			{
				const int32 CurrentScore = TileDistance(ImpactedTile, B);
				if (BestScore > CurrentScore) BestScore = CurrentScore;
			}
			const bool NextIsSecond = Current.Bombed[BombTile.Index()];
			int32 NextCost = Current.Cost + BombCost + BestScore * Weight;
			TArray<FTile> NextBombs = Current.Bombs;
			NextBombs.Emplace(BombTile);
			if ((!NextIsSecond && NextCost < Cost[0][BombTile.Index()]) || (NextIsSecond && NextCost < Cost[1][BombTile.Index()]))
			{
				TArray<bool> NextBombed = Current.Bombed;
				NextBombed[BombTile.Index()] = true;
				ToVisit.emplace(
					Current.StatusMap.PlaceBomb(BombTile, BombPower).DetonateBomb(BombTile),
					NextBombed,
					NextBombs,
					NextCost
				);
			}
		}
	}
	return {};
}

TArray<FTile> FStatusMap::BombImpact(const FTile BombTile, const int32 BombPower) const
{
	TArray<FTile> BombedTiles;
	if (TileStatusMap[BombTile.Index()] == ETileStatus::Wall) return BombedTiles;
	bool Blocked[] = {false, false, false, false};
	constexpr FTile ToAdd[] = {
		{0, -1}, {0, 1}, {-1, 0}, {1, 0}
	};
	BombedTiles.Add(BombTile);
	if (TileStatusMap[BombTile.Index()] == ETileStatus::Reinforced) return BombedTiles;
	for (int i = 1; i <= BombPower; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			if (!Blocked[j])
			{
				FTile BombedTile = BombTile + i * ToAdd[j];
				if (BombedTile.IsValid())
				{
					Blocked[j] |= TileStatusMap[BombedTile.Index()] == ETileStatus::Wall;
					Blocked[j] |= TileStatusMap[BombedTile.Index()] == ETileStatus::Reinforced;
					if (TileStatusMap[BombedTile.Index()] != ETileStatus::Wall) BombedTiles.Add(BombedTile);
				}
			}
		}
	}
	return BombedTiles;
}

FStatusMap FStatusMap::BreakTiles(const TArray<FTile>& ToBreak) const
{
	FStatusMap NewMap = *this;
	for (const FTile Tile : ToBreak)
	{
		if (TileStatusMap[Tile.Index()] == ETileStatus::Breakable)
			NewMap.TileStatusMap[Tile.Index()] = ETileStatus::Safe;
		else if (TileStatusMap[Tile.Index()] == ETileStatus::Bomb)
		{
			NewMap.TileStatusMap[Tile.Index()] = ETileStatus::Safe;
			NewMap.BombPowerMap[Tile.Index()] = 0;
		}
		else if (TileStatusMap[Tile.Index()] == ETileStatus::Reinforced)
			NewMap.TileStatusMap[Tile.Index()] = ETileStatus::Breakable;
	}
	return NewMap;
}

FStatusMap FStatusMap::PlaceBomb(const FTile BombTile, const int32 BombPower) const
{
	FStatusMap NewMap = *this;
	if (!IsBlocked(BombTile))
	{
		NewMap.TileStatusMap[BombTile.Index()] = ETileStatus::Bomb;
		NewMap.BombPowerMap[BombTile.Index()] = BombPower;
	}
	return NewMap;
}

FStatusMap FStatusMap::DetonateBomb(const FTile A) const
{
	TArray<FTile> ImpactedTiles;
	std::queue<FTile> BombTiles;
	if (TileStatusMap[A.Index()] == ETileStatus::Bomb) BombTiles.push(A);
	while (!BombTiles.empty())
	{
		FTile BombTile = BombTiles.front();
		BombTiles.pop();
		for (const FTile ImpactedTile : BombImpact(BombTile, BombPowerMap[BombTile.Index()]))
		{
			ImpactedTiles.AddUnique(ImpactedTile);
			if (ImpactedTile == BombTile) continue;
			if (TileStatusMap[ImpactedTile.Index()] == ETileStatus::Bomb) BombTiles.push(ImpactedTile);
		}
	}
	return BreakTiles(ImpactedTiles);
}

TArray<FTile> FStatusMap::ReachableSafe(const FTile A, const bool Safe) const
{
	TArray<FTile> ReachableSafeTiles;
	for (const FTile ReachableTile : ReachableTiles(A, Safe))
	{
		if (IsSafe(ReachableTile)) ReachableSafeTiles.Add(ReachableTile);
	}
	return ReachableSafeTiles;
}

TArray<FTile> FStatusMap::ViableBombSpots(const FTile A, const int32 BombPower) const
{
	TArray<FTile> BombSpots;
	for (const FTile BombTile : ReachableSafe(A, true))
	{
		for (const FTile ImpactedTile : BombImpact(BombTile, BombPower))
		{
			if (IsBlocked(ImpactedTile))
			{
				BombSpots.Add(BombTile);
				break;
			}
		}
	}
	return BombSpots;
}

TArray<FTile> URobotUtils::GetAStar(const ABomber* Bomber, const FTile Goal, const int32 SafeCost,
	const int32 DangerCost, const int32 Weight)
{
	return FStatusMap(Bomber).AStar(Bomber->CurrentTile, Goal, SafeCost, DangerCost, Weight);
}

TArray<FTile> URobotUtils::GetBombAStar(const ABomber* Bomber, const FTile Goal, const int32 BombPower,
	const int32 BombCost, const int32 Weight)
{
	return FStatusMap(Bomber).BombAStar(Bomber->CurrentTile, Goal, BombPower, BombCost, Weight);
}

FMaybeTile URobotUtils::GetAStarTop(const ABomber* Bomber, const FTile Goal, const int32 SafeCost,
                                   const int32 DangerCost, const int32 Weight)
{
	TArray<FTile> AStarResult = GetAStar(Bomber, Goal, SafeCost, DangerCost, Weight);
	if (AStarResult.Num() > 0) return FMaybeTile::Just(AStarResult[0]);
	return FMaybeTile::None();
}

FMaybeTile URobotUtils::GetBombAStarTop(const ABomber* Bomber, const FTile Goal, const int32 BombPower, const int32 BombCost, const int32 Weight)
{
	TArray<FTile> BombAStarResult = GetBombAStar(Bomber, Goal, BombPower, BombCost, Weight);
	if (BombAStarResult.Num() > 0) return FMaybeTile::Just(BombAStarResult[0]);
	return FMaybeTile::None();
}

bool URobotUtils::IsTileSafe(const ABomber* Bomber, const FTile A)
{
	return FStatusMap(Bomber).IsSafe(A);
}

bool URobotUtils::IsTileBlocked(const ABomber* Bomber, const FTile A)
{
	return FStatusMap(Bomber).IsBlocked(A);
}

bool URobotUtils::IsTileReachable(const ABomber* Bomber, const FTile A, const bool Safe)
{
	return FStatusMap(Bomber).ReachableTiles(Bomber->CurrentTile, Safe).Contains(A);
}
