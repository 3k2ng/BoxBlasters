#include "Libraries/RobotUtils.h"
#include "Actors/PopulatedArena.h"

#include <queue>

TArray<ETileState> RequestTileState(const AArena* Arena)
{
	TArray<ETileState> TileStateMap;
	TileStateMap.Init(ETileState::Normal, GTotal);
	for (int i = 0; i < GTotal; ++i)
	{
		if (Arena->TileMap[i] != ETileType::Empty || Arena->BombTypeMap[i] != EBombType::None || Arena->ExplosionTimerMap[i] > 0.F)
			TileStateMap[i] = ETileState::Blocked;
		else if (Arena->WarningMap[i]) TileStateMap[i] = ETileState::Warning;
	}
	return TileStateMap;
}

TArray<FTile> RequestReachableTiles(const TArray<ETileState>& TileStateMap, const FTile From, const bool Safe = true)
{
	TArray<FTile> ReachableTiles;
	TArray<bool> Visited;
	Visited.Init(false, GTotal);
	std::queue<FTile> ToVisit;
	ToVisit.push(From);
	while (!ToVisit.empty())
	{
		FTile Current = ToVisit.front();
		ToVisit.pop();
		if (Visited[Current.Index()]) continue;
		Visited[Current.Index()] = true;
		for (const FTile Neighbor : Current.Neighbors())
		{
			if ((TileStateMap[Neighbor.Index()] == ETileState::Normal) ||
				(!Safe && TileStateMap[Neighbor.Index()] == ETileState::Warning))
			{
				ReachableTiles.Add(Neighbor);
				ToVisit.push(Neighbor);
			}
		}
	}
	return ReachableTiles;
}

TArray<FTile> RequestBombEscapes(const TArray<ETileState>& TileStateMap, const FTile From)
{
	TArray<FTile> BombEscapes;
	for (const FTile BombEscape : RequestReachableTiles(TileStateMap, From, false))
	{
		if (TileStateMap[BombEscape.Index()] == ETileState::Normal) BombEscapes.Add(BombEscape);
	}
	return BombEscapes;
}

int32 RequestTileCost(const TArray<ETileState>& TileStateMap, const FTile From, const FTile To,
                      const int32 NormalCost, const int32 WarningCost)
{
	TArray<int32> Cost;
	Cost.Init(MAX_int32, GTotal);
	std::priority_queue<std::pair<int32, FTile>, std::vector<std::pair<int32, FTile>>, std::greater<>> ToVisit;
	ToVisit.emplace(0, From);
	while (!ToVisit.empty())
	{
		const int32 CurrentCost = ToVisit.top().first;
		const FTile CurrentTile = ToVisit.top().second;
		ToVisit.pop();
		if (CurrentCost > Cost[CurrentTile.Index()]) continue;
		Cost[CurrentTile.Index()] = CurrentCost;
		if (CurrentTile == To)
		{
			return CurrentCost;
		}
		for (const FTile Neighbor : CurrentTile.Neighbors())
		{
			if (TileStateMap[Neighbor.Index()] == ETileState::Normal)
			{
				int32 NewCost = CurrentCost + NormalCost;
				if (NewCost < Cost[Neighbor.Index()])
				{
					ToVisit.emplace(NewCost, Neighbor);
				}
			}
			else if (TileStateMap[Neighbor.Index()] == ETileState::Warning)
			{
				int32 NewCost = CurrentCost + WarningCost;
				if (NewCost < Cost[Neighbor.Index()])
				{
					ToVisit.emplace(NewCost, Neighbor);
				}
			}
		}
	}
	return MAX_int32;
}

FMaybeTile RequestLeastCostTile(const TArray<ETileState>& TileStateMap, const FTile From, const TArray<FTile>& To,
                             const int32 NormalCost, const int32 WarningCost)
{
	if (To.Num() > 0)
	{
		TArray<int32> Cost;
		Cost.Init(MAX_int32, GTotal);
		std::priority_queue<std::pair<int32, FTile>, std::vector<std::pair<int32, FTile>>, std::greater<>> ToVisit;
		ToVisit.emplace(0, From);
		while (!ToVisit.empty())
		{
			const int32 CurrentCost = ToVisit.top().first;
			const FTile CurrentTile = ToVisit.top().second;
			ToVisit.pop();
			if (CurrentCost > Cost[CurrentTile.Index()]) continue;
			Cost[CurrentTile.Index()] = CurrentCost;
			if (To.Contains(CurrentTile))
			{
				return FMaybeTile::Just(CurrentTile);
			}
			for (const FTile Neighbor : CurrentTile.Neighbors())
			{
				if (TileStateMap[Neighbor.Index()] == ETileState::Normal)
				{
					int32 NewCost = CurrentCost + NormalCost;
					if (NewCost < Cost[Neighbor.Index()])
					{
						ToVisit.emplace(NewCost, Neighbor);
					}
				}
				else if (TileStateMap[Neighbor.Index()] == ETileState::Warning)
				{
					int32 NewCost = CurrentCost + WarningCost;
					if (NewCost < Cost[Neighbor.Index()])
					{
						ToVisit.emplace(NewCost, Neighbor);
					}
				}
			}
		}
	}
	return FMaybeTile::None();
}

FMaybeTile URobotUtils::Dijkstra(const APopulatedArena* Arena, const FTile From, const FTile To, const int32 NormalCost,
                              const int32 WarningCost)
{
	CHECK_VALID(Arena)
	TArray<ETileState> TileStateMap = RequestTileState(Arena);
	for (int32 i = 0; i < 4; ++i)
	{
		if (IsValid(Arena->Bombers[i])) TileStateMap[Arena->Bombers[i]->CurrentTile.Index()] = ETileState::Blocked;
	}
	TArray<int32> Cost;
	Cost.Init(MAX_int32, GTotal);
	std::priority_queue<FPathFindStep> ToVisit;
	for (FTile Origin : From.Neighbors())
	{
		if (TileStateMap[Origin.Index()] == ETileState::Normal)
		{
			Cost[Origin.Index()] = NormalCost;
			ToVisit.emplace(NormalCost, Origin, Origin);
		}
		else if (TileStateMap[Origin.Index()] == ETileState::Warning)
		{
			Cost[Origin.Index()] = WarningCost;
			ToVisit.emplace(WarningCost, Origin, Origin);
		}
	}
	while (!ToVisit.empty())
	{
		const auto Current = ToVisit.top();
		ToVisit.pop();
		if (Current.Cost > Cost[Current.Target.Index()]) continue;
		Cost[Current.Target.Index()] = Current.Cost;
		if (Current.Target == To)
		{
			return FMaybeTile::Just(Current.Origin);
		}
		for (const FTile Neighbor : Current.Target.Neighbors())
		{
			if (TileStateMap[Neighbor.Index()] == ETileState::Normal)
			{
				int32 NewCost = Current.Cost + NormalCost;
				if (NewCost < Cost[Neighbor.Index()])
				{
					ToVisit.emplace(NewCost, Neighbor, Current.Origin);
				}
			}
			else if (TileStateMap[Neighbor.Index()] == ETileState::Warning)
			{
				int32 NewCost = Current.Cost + WarningCost;
				if (NewCost < Cost[Neighbor.Index()])
				{
					ToVisit.emplace(NewCost, Neighbor, Current.Origin);
				}
			}
		}
	}
	return FMaybeTile::None();
}

bool URobotUtils::IsWarningAt(const AArena* Arena, const FTile Tile)
{
	CHECK_VALID(Arena);
	return Arena->WarningMap[Tile.Index()];
}

bool URobotUtils::IsBlockedAt(const AArena* Arena, const FTile Tile)
{
	CHECK_VALID(Arena);
	return Arena->TileMap[Tile.Index()] != ETileType::Empty || Arena->BombTypeMap[Tile.Index()] != EBombType::None || Arena->ExplosionTimerMap[Tile.Index()] > 0.F;
}

int32 URobotUtils::GetDistance(const FTile From, const FTile To)
{
	if (From.X == To.X && From.X % 2 == 1)
	{
		return 2 + abs(From.Y - To.Y);
	}
	else if (From.Y == To.Y && From.Y % 2 == 1)
	{
		return 2 + abs(From.X - To.X);
	}
	return abs(From.X - To.X) + abs(From.Y - To.Y);
}

TArray<FTile> URobotUtils::FindBoxes(const AArena* Arena,
                                     const bool IncludeWhite,
                                     const bool IncludeRed,
                                     const bool IncludeGreen,
                                     const bool IncludeBlue)
{
	CHECK_VALID(Arena);
	TArray<FTile> FoundBoxes;
	for (int i = 0; i < GTotal; ++i)
	{
		if ((IncludeWhite && Arena->TileMap[i] == ETileType::White) ||
			(IncludeRed && Arena->TileMap[i] == ETileType::Red) ||
			(IncludeGreen && Arena->TileMap[i] == ETileType::Green) ||
			(IncludeBlue && Arena->TileMap[i] == ETileType::Blue))
			FoundBoxes.Add(IndexTile(i));
	}
	return FoundBoxes;
}

EBombType URobotUtils::GetBombType(const AArena* Arena, const FTile Target)
{
	CHECK_VALID(Arena);
	return Arena->BombTypeMap[Target.Index()];
}

TArray<FTile> URobotUtils::GetReachableTiles(const APopulatedArena* Arena, const FTile From, const bool Safe = true)
{
	CHECK_VALID(Arena);
	TArray<ETileState> TileStateMap = RequestTileState(Arena);
	for (int32 i = 0; i < 4; ++i)
	{
		if (IsValid(Arena->Bombers[i])) TileStateMap[Arena->Bombers[i]->CurrentTile.Index()] = ETileState::Blocked;
	}
	return RequestReachableTiles(TileStateMap, From, Safe);
}

FMaybeTile URobotUtils::GetNearestTile(const FTile From, const TArray<FTile>& To)
{
	if (To.Num() > 0)
	{
		FTile NearestTile = To[0];
		int32 MinDistance = GetDistance(From, To[0]);
		for (int i = 1; i < To.Num(); ++i)
		{
			const FTile CurrentTile = To[i];
			const int32 CurrentDistance = GetDistance(From, To[i]);
			if (CurrentDistance < MinDistance)
			{
				NearestTile = CurrentTile;
				MinDistance = CurrentDistance;
			}
		}
		return FMaybeTile::Just(NearestTile);
	}
	return FMaybeTile::None();
}

TArray<FTile> URobotUtils::BombSpotsToHit(const ABomber* Bomber, const FTile Target)
{
	CHECK_VALID(Bomber);
	CHECK_VALID(Bomber->Arena);
	const APopulatedArena* Arena = Cast<APopulatedArena>(Bomber->Arena);
	CHECK_VALID(Arena);
	TArray<ETileState> TileStateMap = RequestTileState(Arena);
	for (int32 i = 0; i < 4; ++i)
	{
		if (IsValid(Arena->Bombers[i])) TileStateMap[Arena->Bombers[i]->CurrentTile.Index()] = ETileState::Blocked;
	}
	TArray<FTile> BombSpots;
	for (const FTile BombSpot : Arena->GetBombedTiles(Target, Bomber->Power))
	{
		if (TileStateMap[BombSpot.Index()] == ETileState::Normal) BombSpots.Add(BombSpot);
	}
	return BombSpots;
}

TArray<FTile> URobotUtils::GetPotentialBombEscapes(const ABomber* Bomber, const FTile Target)
{
	CHECK_VALID(Bomber);
	CHECK_VALID(Bomber->Arena);
	const APopulatedArena* Arena = Cast<APopulatedArena>(Bomber->Arena);
	CHECK_VALID(Arena);
	TArray<ETileState> TileStateMap = RequestTileState(Arena);
	for (int32 i = 0; i < 4; ++i)
	{
		if (i != Bomber->Index && IsValid(Arena->Bombers[i]))
			TileStateMap[Arena->Bombers[i]->CurrentTile.Index()] =
				ETileState::Blocked;
	}
	for (const FTile BombedTile : Arena->GetBombedTiles(Target, Bomber->Power))
	{
		if (TileStateMap[BombedTile.Index()] == ETileState::Normal)
			TileStateMap[BombedTile.Index()] = ETileState::Warning;
	}
	TArray<FTile> BombEscapes;
	for (const FTile BombEscape : RequestReachableTiles(TileStateMap, Target, false))
	{
		if (TileStateMap[BombEscape.Index()] == ETileState::Normal) BombEscapes.Add(BombEscape);
	}
	return BombEscapes;
}

TArray<FTile> URobotUtils::GetCurrentBombEscapes(const ABomber* Bomber)
{
	CHECK_VALID(Bomber);
	CHECK_VALID(Bomber->Arena);
	const APopulatedArena* Arena = Cast<APopulatedArena>(Bomber->Arena);
	CHECK_VALID(Arena);
	TArray<ETileState> TileStateMap = RequestTileState(Arena);
	for (int32 i = 0; i < 4; ++i)
	{
		if (i != Bomber->Index && IsValid(Arena->Bombers[i]))
			TileStateMap[Arena->Bombers[i]->CurrentTile.Index()] =
				ETileState::Blocked;
	}
	TArray<FTile> BombEscapes;
	for (const FTile BombEscape : RequestReachableTiles(TileStateMap, Bomber->CurrentTile, false))
	{
		if (TileStateMap[BombEscape.Index()] == ETileState::Normal) BombEscapes.Add(BombEscape);
	}
	return BombEscapes;
}

bool URobotUtils::CanReachTile(const ABomber* Bomber, const FTile Target, const bool Safe = true)
{
	CHECK_VALID(Bomber);
	CHECK_VALID(Bomber->Arena);
	const APopulatedArena* Arena = Cast<APopulatedArena>(Bomber->Arena);
	CHECK_VALID(Arena);
	return GetReachableTiles(Arena, Bomber->CurrentTile, Safe).Contains(Target);
}
