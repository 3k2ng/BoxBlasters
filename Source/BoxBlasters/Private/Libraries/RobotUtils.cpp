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
		switch (Arena->TileMap[i]) {
		case ETileType::Empty:
			switch (Arena->BombTypeMap[i]) {
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

TArray<FTile> URobotUtils::Dijkstra(const APopulatedArena* Arena, const FTile From, const FTile To, const int32 NormalCost, const int32 WarningCost)
{
	CHECK_VALID(Arena)
	TArray<ETileState> TileStateMap = RequestTileState(Arena);
	for (int32 i = 0; i < 4; ++i)
	{
		if (IsValid(Arena->Bombers[i])) TileStateMap[Arena->Bombers[i]->CurrentTile.Index()] = ETileState::Blocked;
	}
	TArray<FTile> Parent;
	for (int32 i = 0; i < GTotal; ++i) Parent.Add(IndexTile(i));
	TArray<int32> Cost;
	Cost.Init(MAX_int32, GTotal);
	std::priority_queue<std::pair<int32, FTile>, std::vector<std::pair<int32, FTile>>, std::greater<>> ToVisit;
	ToVisit.emplace(0, From);
	while (!ToVisit.empty())
	{
		const int32 CCost = ToVisit.top().first;
		const FTile CTile = ToVisit.top().second;
		ToVisit.pop();
		if (CCost > Cost[CTile.Index()]) continue;
		Cost[CTile.Index()] = CCost;
		if (CTile == To)
		{
			TArray<FTile> RevPath;
			FTile Trace = To;
			while (Trace != From)
			{
				RevPath.Add(Trace);
				Trace = Parent[Trace.Index()];
			}
			TArray<FTile> Path;
			for (int32 i = RevPath.Num() - 1; i >= 0; --i)
			{
				Path.Add(RevPath[i]);
			}
			return Path;
		}
		for (const FTile Neighbor : CTile.Neighbors())
		{
			if (TileStateMap[Neighbor.Index()] == ETileState::Normal)
			{
				int32 NewCost = CCost + NormalCost;
				if (NewCost < Cost[Neighbor.Index()])
				{
					Parent[Neighbor.Index()] = CTile;
					ToVisit.emplace(NewCost, Neighbor);
				}
			}
			else if (TileStateMap[Neighbor.Index()] == ETileState::Warning)
			{
				int32 NewCost = CCost + WarningCost;
				if (NewCost < Cost[Neighbor.Index()])
				{
					Parent[Neighbor.Index()] = CTile;
					ToVisit.emplace(NewCost, Neighbor);
				}
			}
		}
	}
	return {};
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
