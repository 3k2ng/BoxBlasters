#include "Libraries/PlannerUtils.h"

FMaybePlan UPlannerUtils::BestPlan(const TArray<FRobotPlan>& Plans)
{
	FMaybePlan PickedPlan = FMaybePlan::None();
	for (const auto& Plan : Plans)
	{
		if (!PickedPlan.HasPlan || (PickedPlan.HasPlan && PickedPlan.JustPlan.Cost > Plan.Cost))
			PickedPlan = {true, Plan};
	}
	return PickedPlan;
}
