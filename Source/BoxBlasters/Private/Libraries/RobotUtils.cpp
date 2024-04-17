#include "Libraries/RobotUtils.h"
#include "Actors/PopulatedArena.h"

#include <queue>

TArray<ETileState> RequestTileState(const AArena* Arena)
{
	TArray<ETileState> TileStateMap;
	TileStateMap.Init(ETileState::Normal, GTotal);
	for (int i = 0; i < GTotal; ++i)
	{
		if (IsValid(Arena->AreaObjectMap[i]))
		{
			if (Arena->AreaObjectMap[i]->Blocked) TileStateMap[i] = ETileState::Blocked;
			else if (Arena->AreaObjectMap[i]->Warning) TileStateMap[i] = ETileState::Warning;
		}
		else TileStateMap[i] = ETileState::Blocked;
	}
	return TileStateMap;
}

TArray<FTile> RequestReachableTiles(const TArray<ETileState>& TileStateMap, const FTile From, const bool NoWarning = true)
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
				(!NoWarning && TileStateMap[Neighbor.Index()] == ETileState::Warning))
			{
				ReachableTiles.Add(Neighbor);
				ToVisit.push(Neighbor);
			}
		}
	}
	return ReachableTiles;
}

struct FPathFindStep
{
	int32 Cost;
	FTile Target;
	FTile Origin;

	FPathFindStep(const int32 InCost,
	              const FTile InTarget,
	              const FTile InOrigin) : Cost(InCost), Target(InTarget), Origin(InOrigin)
	{
	}
};

inline bool operator<(FPathFindStep const& A, FPathFindStep const& B)
{
	return A.Cost > B.Cost;
}

FResult URobotUtils::Dijkstra(const APopulatedArena* Arena, const FTile From, const FTile To, const int32 NormalCost,
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
			return FResult::Success(Current.Origin);
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
	return FResult::Fail();
}

bool URobotUtils::IsWarningAt(const AArena* Arena, const FTile Tile)
{
	CHECK_VALID(Arena);
	if (IsValid(Arena->AreaObjectMap[Tile.Index()])) return Arena->AreaObjectMap[Tile.Index()]->Warning;
	return false;
}

bool URobotUtils::IsBlockedAt(const AArena* Arena, const FTile Tile)
{
	CHECK_VALID(Arena);
	if (IsValid(Arena->AreaObjectMap[Tile.Index()])) return Arena->AreaObjectMap[Tile.Index()]->Blocked;
	return true;
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

TArray<FTile> URobotUtils::GetReachableTiles(const APopulatedArena* Arena, const FTile From, const bool NoWarning = true)
{
	CHECK_VALID(Arena);
	TArray<ETileState> TileStateMap = RequestTileState(Arena);
	for (int32 i = 0; i < 4; ++i)
	{
		if (IsValid(Arena->Bombers[i])) TileStateMap[Arena->Bombers[i]->CurrentTile.Index()] = ETileState::Blocked;
	}
	return RequestReachableTiles(TileStateMap, From, NoWarning);
}

FResult URobotUtils::GetNearestTile(const FTile From, const TArray<FTile>& To)
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
		return FResult::Success(NearestTile);
	}
	return FResult::Fail();
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
		if (i != Bomber->Index && IsValid(Arena->Bombers[i])) TileStateMap[Arena->Bombers[i]->CurrentTile.Index()] = ETileState::Blocked;
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
		if (i != Bomber->Index && IsValid(Arena->Bombers[i])) TileStateMap[Arena->Bombers[i]->CurrentTile.Index()] = ETileState::Blocked;
	}
	TArray<FTile> BombEscapes;
	for (const FTile BombEscape : RequestReachableTiles(TileStateMap, Bomber->CurrentTile, false))
	{
		if (TileStateMap[BombEscape.Index()] == ETileState::Normal) BombEscapes.Add(BombEscape);
	}
	return BombEscapes;
}

FResult URobotUtils::BestBombSpotsToReach(const ABomber* Bomber, const FTile Target)
{
	CHECK_VALID(Bomber);
	CHECK_VALID(Bomber->Arena);
	const APopulatedArena* Arena = Cast<APopulatedArena>(Bomber->Arena);
	CHECK_VALID(Arena);
	TArray<ETileState> TileStateMap = RequestTileState(Arena);
	for (int32 i = 0; i < 4; ++i)
	{
		if (i != Bomber->Index && IsValid(Arena->Bombers[i]))
			TileStateMap[Arena->Bombers[i]->CurrentTile.Index()] = ETileState::Blocked;
	}
	TArray<FTile> BombSpots = RequestReachableTiles(TileStateMap, Bomber->CurrentTile);
	BombSpots.AddUnique(Bomber->CurrentTile);
	if (BombSpots.Num() > 0)
	{
		FResult BestSpot = FResult::Fail();
		int32 BestScore = MAX_int32;
		for (const FTile BombSpot : BombSpots)
		{
			int32 CurrentBestScore = MAX_int32;
			for (const FTile BombedTile : Arena->GetBombedTiles(BombSpot, Bomber->Power))
			{
				if (Arena->TileMap[BombedTile.Index()] != ETileType::Empty)
				{
					const int32 CurrentScore = GetDistance(BombedTile, Target);
					if (CurrentBestScore > CurrentScore) CurrentBestScore = CurrentScore;
				}
			}
			if (BestScore > CurrentBestScore && GetPotentialBombEscapes(Bomber, BombSpot).Num() > 0)
			{
				BestSpot = FResult::Success(BombSpot);
				BestScore = CurrentBestScore;
			}
		}
		return BestSpot;
	}
	return FResult::Fail();
}

bool URobotUtils::CanReachTile(const ABomber* Bomber, const FTile Target, const bool NoWarning = true)
{
	CHECK_VALID(Bomber);
	CHECK_VALID(Bomber->Arena);
	const APopulatedArena* Arena = Cast<APopulatedArena>(Bomber->Arena);
	CHECK_VALID(Arena);
	return GetReachableTiles(Arena, Bomber->CurrentTile, NoWarning).Contains(Target);
}
