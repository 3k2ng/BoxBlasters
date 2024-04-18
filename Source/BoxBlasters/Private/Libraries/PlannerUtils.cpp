#include "Libraries/PlannerUtils.h"

#include <queue>
#include <string>

void LogTileState(const TArray<ETileState>& TileStateMap)
{
	std::string LogResult;
	for (int i = 0; i < GY; ++i)
	{
		for (int j = 0; j < GX; ++j)
		{
			switch (TileStateMap[FTile{j, i}.Index()]) {
			case ETileState::Normal:
				LogResult += ".";
				break;
			case ETileState::Warning:
				LogResult += "_";
				break;
			case ETileState::Blocked:
				LogResult += "#";
				break;
			}
		}
		LogResult += "\n";
	}
	UE_LOG(LogTemp, Log, TEXT("%hs"), LogResult.c_str());
}

void LogMapState(const FMapState& MapState)
{
	std::string LogResult;
	for (int i = 0; i < GY; ++i)
	{
		for (int j = 0; j < GX; ++j)
		{
			if (MapState.TileMap[FTile{j, i}.Index()] == ETileType::Empty)
			{
				if (MapState.BombTypeMap[FTile{j, i}.Index()] != EBombType::None) LogResult += "%";
				else LogResult += ".";
			}
			else if (MapState.TileMap[FTile{j, i}.Index()] == ETileType::Wall) LogResult += "#";
			else LogResult += "$";
		}
		LogResult += "\n";
	}
	UE_LOG(LogTemp, Log, TEXT("%hs"), LogResult.c_str());
}

FMapState::FMapState(const AArmedBomber* Bomber)
{
	CHECK_VALID(Bomber);
	CHECK_VALID(Bomber->Arena);
	const APopulatedArena* Arena = Cast<APopulatedArena>(Bomber->Arena);
	CHECK_VALID(Arena);
	TileMap = Arena->TileMap;
	BombTypeMap = Arena->BombTypeMap;
	BombPowerMap = Arena->BombPowerMap;
	for (int i = 0; i < 4; ++i)
	{
		BomberAlive[i] = false;
		if (IsValid(Arena->Bombers[i]))
		{
			BomberTile[i] = Arena->Bombers[i]->CurrentTile;
			BomberAlive[i] = true;
		}
	}
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

TArray<FBombingPathFindStep> FBombingPathFindStep::GetNext(const FTile Target, const int32 BombPower) const
{
	const TArray<ETileState> TileStateMap = MapState.TileState();
	TArray<FBombingPathFindStep> NextSteps;
	for (const auto BombTile : RequestReachableTiles(TileStateMap, Location, true))
	{
		int32 NewCost = Cost + RequestTileCost(TileStateMap, Location, BombTile, 1, 100);
		TArray<FRobotTask> NewTaskList = TaskList;
		FMapState BombedMapState = MapState.PlaceBomb(BombTile, EBombType::Normal, BombPower);
		const TArray<ETileState> BombedTileStateMap = BombedMapState.TileState();
		TArray<FTile> BombEscapes = RequestBombEscapes(BombedTileStateMap, BombTile);
		const FMaybeTile BestEscape = RequestLeastCostTile(BombedTileStateMap, BombTile, BombEscapes, 1, 100);
		if (BestEscape.HasTile)
		{
			NewCost += RequestTileCost(TileStateMap, BombTile, BestEscape.JustTile, 1, 100);
			NewTaskList.Append({
				{ ERobotTaskType::Wait, BombTile },
				{ ERobotTaskType::Move, BombTile },
				{ ERobotTaskType::Bomb, {} },
				{ ERobotTaskType::Move, BestEscape.JustTile },
				{ ERobotTaskType::Wait, BombTile },
			});
			const FTile NewLocation = BestEscape.JustTile;
			const FMapState DetonatedMapState = BombedMapState.DetonateBomb(BombTile);
			const TArray<ETileState> DetonatedTileStateMap = DetonatedMapState.TileState();
			int32 BestScore = URobotUtils::GetDistance(NewLocation, Target);
			for (const auto NewReachableTile : RequestReachableTiles(TileStateMap, NewLocation, false))
			{
				const int32 CurrentScore = URobotUtils::GetDistance(NewReachableTile, Target);
				if (BestScore > CurrentScore) BestScore = CurrentScore;
			}
			NewCost += 10000 * BestScore;
			NewCost += 10000;
			NextSteps.Add({
				NewCost,
				NewLocation,
				DetonatedMapState,
				NewTaskList
			});
		}
	}
	NextSteps.Sort([](const FBombingPathFindStep& A, const FBombingPathFindStep& B){ return A.Cost > B.Cost; });
	TArray<FBombingPathFindStep> ChosenSteps;
	for (int i = 0; i < std::min(3, NextSteps.Num()); ++i)
	{
		ChosenSteps.Push(NextSteps[i]);
	}
	return ChosenSteps;
}

FMaybePlan UPlannerUtils::PlanToReach(const AArmedBomber* Bomber, const FTile Target)
{
	const FMapState InitMapState(Bomber);
	const FBombingPathFindStep InitStep{
		0,
		Bomber->CurrentTile,
		InitMapState,
		{}
	};
	std::priority_queue<FBombingPathFindStep> Queue;
	Queue.push(InitStep);
	while (!Queue.empty())
	{
		auto Current = Queue.top();
		UE_LOG(LogTemp, Log, TEXT("--------------------------------------------------------------------------"));
		LogMapState(Current.MapState);
		Queue.pop();
		if (RequestReachableTiles(Current.MapState.TileState(), Bomber->CurrentTile, false).Contains(Target))
		{
			return FMaybePlan::Just({ Current.TaskList });
		}
		for (const auto Next : Current.GetNext(Target, Bomber->Power))
		{
			Queue.push(Next);
		}
	}
	return FMaybePlan::None();
}
