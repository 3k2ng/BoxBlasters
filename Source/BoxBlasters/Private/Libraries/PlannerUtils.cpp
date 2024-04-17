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
