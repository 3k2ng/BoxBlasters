#include "Libraries/PlannerUtils.h"

bool FRobotPlan::IsSatisfied(const FRobotState& InState) const
{
	for (const auto& Cond : Precondition.RobotCond.CondSet)
	{
		if (!InState.RobotCond.CondSet.Contains(Cond)) return false;
	}
	for (int i = 0; i < GTotal; ++i)
	{
		for (const auto& Cond : Precondition.TileCond[i].CondSet)
		{
			if (!InState.TileCond[i].CondSet.Contains(Cond)) return false;
		}
	}
	for (int i = 0; i < 4; ++i)
	{
		for (const auto& Cond : Precondition.EnemyCond[i].CondSet)
		{
			if (!InState.EnemyCond[i].CondSet.Contains(Cond)) return false;
		}
	}
	return true;
}

FRobotState FRobotPlan::Transform(const FRobotState& InState) const
{
	FRobotState NewState = InState;
	for (const auto& Cond : EffectRemove.RobotCond.CondSet)
	{
		NewState.RobotCond.CondSet.Remove(Cond);
	}
	for (int i = 0; i < GTotal; ++i)
	{
		for (const auto& Cond : EffectRemove.TileCond[i].CondSet)
		{
			NewState.TileCond[i].CondSet.Remove(Cond);
		}
	}
	for (int i = 0; i < 4; ++i)
	{
		for (const auto& Cond : EffectRemove.EnemyCond[i].CondSet)
		{
			NewState.EnemyCond[i].CondSet.Remove(Cond);
		}
	}
	for (const auto& Cond : EffectAdd.RobotCond.CondSet)
	{
		NewState.RobotCond.CondSet.AddUnique(Cond);
	}
	for (int i = 0; i < GTotal; ++i)
	{
		for (const auto& Cond : EffectAdd.TileCond[i].CondSet)
		{
			NewState.TileCond[i].CondSet.AddUnique(Cond);
		}
	}
	for (int i = 0; i < 4; ++i)
	{
		for (const auto& Cond : EffectAdd.EnemyCond[i].CondSet)
		{
			NewState.EnemyCond[i].CondSet.AddUnique(Cond);
		}
	}
	return NewState;
}

FRobotState UPlannerUtils::QueryState(const AArmedBomber* Bomber)
{
	CHECK_VALID(Bomber);
	CHECK_VALID(Bomber->Arena);
	const APopulatedArena* Arena = Cast<APopulatedArena>(Bomber->Arena);
	CHECK_VALID(Arena);
	FRobotState RobotState;
	if (!URobotUtils::IsWarningAt(Arena, Bomber->CurrentTile)) RobotState.AddRobotCond(ERobotCond::Safe);
	if (Bomber->IsBombAvailable()) RobotState.AddRobotCond(ERobotCond::HasBomb);
	switch (Bomber->EquippedSpecial)
	{
	case ESpecialType::None:
		break;
	case ESpecialType::Remote:
		RobotState.AddRobotCond(ERobotCond::HasRemote);
		break;
	case ESpecialType::Mine:
		RobotState.AddRobotCond(ERobotCond::HasMine);
		break;
	case ESpecialType::Air:
		RobotState.AddRobotCond(ERobotCond::HasAir);
		break;
	}
	const auto TileStateMap = RequestTileState(Arena);
	const auto ReachableTiles = RequestReachableTiles(TileStateMap, Bomber->CurrentTile, false);
	for (int32 i = 0; i < GTotal; ++i)
	{
		if (TileStateMap[i] == ETileState::Normal) RobotState.AddTileCond(IndexTile(i), ETileCond::Safe);
		if (ReachableTiles.Contains(IndexTile(i))) RobotState.AddTileCond(IndexTile(i), ETileCond::Reachable);
		switch (Arena->TileMap[i]) {
		case ETileType::Empty:
			RobotState.AddTileCond(IndexTile(i), ETileCond::Empty);
			break;
		case ETileType::Wall:
			RobotState.AddTileCond(IndexTile(i), ETileCond::Wall);
			break;
		case ETileType::Basic:
			RobotState.AddTileCond(IndexTile(i), ETileCond::Basic);
			break;
		case ETileType::Reinforced:
			RobotState.AddTileCond(IndexTile(i), ETileCond::Reinforced);
			break;
		case ETileType::White:
			RobotState.AddTileCond(IndexTile(i), ETileCond::White);
			break;
		case ETileType::Red:
			RobotState.AddTileCond(IndexTile(i), ETileCond::Red);
			break;
		case ETileType::Green:
			RobotState.AddTileCond(IndexTile(i), ETileCond::Green);
			break;
		case ETileType::Blue:
			RobotState.AddTileCond(IndexTile(i), ETileCond::Blue);
			break;
		}
		switch (Arena->BombTypeMap[i]) {
		case EBombType::None:
			break;
		case EBombType::Normal:
			RobotState.AddTileCond(IndexTile(i), ETileCond::HasBomb);
			break;
		case EBombType::Remote:
			RobotState.AddTileCond(IndexTile(i), ETileCond::HasRemote);
			break;
		case EBombType::Mine:
			RobotState.AddTileCond(IndexTile(i), ETileCond::HasMine);
			break;
		}
	}
	for (int32 i = 0; i < 4; ++i)
	{
		if (IsValid(Arena->Bombers[i]))
		{
			RobotState.AddEnemyCond(i, EEnemyCond::Alive);
			if (i != Bomber->Index) RobotState.AddEnemyCond(i, EEnemyCond::IsEnemy);
		}
	}
	return RobotState;
}
