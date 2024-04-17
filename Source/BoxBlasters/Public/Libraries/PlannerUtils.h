#pragma once

#include "CoreMinimal.h"
#include "CommonUtils.h"
#include "RobotUtils.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PlannerUtils.generated.h"

USTRUCT(BlueprintType)
struct FRobotPlan
{
	GENERATED_BODY()
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 Cost;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FRobotTask> TaskList;
};

USTRUCT(BlueprintType)
struct FMaybePlan
{
	GENERATED_BODY()
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool HasPlan;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FRobotPlan JustPlan;

	static FMaybePlan None() { return {false, {}}; }
	static FMaybePlan Just(const FRobotPlan& InPlan) { return {true, InPlan}; }
};

UCLASS()
class BOXBLASTERS_API UPlannerUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
};
