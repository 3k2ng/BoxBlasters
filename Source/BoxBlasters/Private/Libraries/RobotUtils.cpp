#include "Libraries/RobotUtils.h"
#include "Actors/PopulatedArena.h"

#include <queue>

TArray<ETileState> RequestTileState(const AArena* Arena)
{
	TArray<ETileState> TileState;
	TileState.Init(ETileState::Normal, GTotal);
	for (int i = 0; i < GTotal; ++i)
	{
		if (IsValid(Arena->AreaObjectMap[i]))
		{
			if (Arena->AreaObjectMap[i]->Blocked) TileState[i] = ETileState::Blocked;
			else if (Arena->AreaObjectMap[i]->Warning) TileState[i] = ETileState::Warning;
		}
		else TileState[i] = ETileState::Blocked;
	}
	return TileState;
}

TArray<ETileInfo> RequestTileInfo(const AArena* Arena)
{
	TArray<ETileInfo> TileInfo;
	TileInfo.Init(ETileInfo::Empty, GTotal);
	for (int i = 0; i < GTotal; ++i)
	{
		switch (Arena->TileMap[i])
		{
		case ETileType::Empty:
			switch (Arena->BombTypeMap[i])
			{
			case EBombType::None:
				TileInfo[i] = ETileInfo::Empty;
				break;
			case EBombType::Normal:
				TileInfo[i] = ETileInfo::NormalBomb;
				break;
			case EBombType::Remote:
				TileInfo[i] = ETileInfo::RemoteBomb;
				break;
			case EBombType::Mine:
				TileInfo[i] = ETileInfo::Mine;
				break;
			}
			break;
		case ETileType::Wall:
			TileInfo[i] = ETileInfo::Wall;
			break;
		case ETileType::Basic:
			TileInfo[i] = ETileInfo::Basic;
			break;
		case ETileType::Reinforced:
			TileInfo[i] = ETileInfo::Reinforced;
			break;
		case ETileType::White:
			TileInfo[i] = ETileInfo::White;
			break;
		case ETileType::Red:
			TileInfo[i] = ETileInfo::Red;
			break;
		case ETileType::Green:
			TileInfo[i] = ETileInfo::Green;
			break;
		case ETileType::Blue:
			TileInfo[i] = ETileInfo::Blue;
			break;
		}
	}
	return TileInfo;
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
