#pragma once

#include "CoreMinimal.h"
#include "CommonUtils.h"
#include "RobotUtils.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PlannerUtils.generated.h"

UENUM(BlueprintType)
enum class ERobotCond : uint8
{
	Safe UMETA(DisplayName = "Safe"),
	HasBomb UMETA(DisplayName = "Has Bomb"),
	HasRemote UMETA(DisplayName = "Has Remote"),
	HasMine UMETA(DisplayName = "Has Mine"),
	HasAir UMETA(DisplayName = "Has Air"),
};

USTRUCT(BlueprintType)
struct FRobotCondSet
{
	GENERATED_BODY()
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<ERobotCond> CondSet;
};

UENUM(BlueprintType)
enum class ETileCond : uint8
{
	Safe UMETA(DisplayName = "Safe"),
	Reachable UMETA(DisplayName = "Reachable"),
	InBombRange UMETA(DisplayName = "In Bomb Range"),
	InRemoteRange UMETA(DisplayName = "In Remote Range"),
	InAirRange UMETA(DisplayName = "In Air Range"),
	Empty UMETA(DisplayName = "Empty"),
	Basic UMETA(DisplayName = "Basic"),
	Reinforced UMETA(DisplayName = "Reinforced"),
	White UMETA(DisplayName = "White"),
	Red UMETA(DisplayName = "Red"),
	Green UMETA(DisplayName = "Green"),
	Blue UMETA(DisplayName = "Blue"),
	HasBomb UMETA(DisplayName = "Has Bomb"),
	HasRemote UMETA(DisplayName = "Has Remote"),
	HasMine UMETA(DisplayName = "Has Mine"),
	HasRedSpecial UMETA(DisplayName = "Has Red Special"),
	HasGreenSpecial UMETA(DisplayName = "Has Green Special"),
	HasBlueSpecial UMETA(DisplayName = "Has Blue Special"),
	HasRedUpgrade UMETA(DisplayName = "Has Red Upgrade"),
	HasGreenUpgrade UMETA(DisplayName = "Has Green Upgrade"),
	HasBlueUpgrade UMETA(DisplayName = "Has Blue Upgrade"),
};

USTRUCT(BlueprintType)
struct FTileCondSet
{
	GENERATED_BODY()
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<ETileCond> CondSet;
};

UENUM(BlueprintType)
enum class EEnemyCond : uint8
{
	Alive UMETA(DisplayName = "Alive"),
	IsEnemy UMETA(DisplayName = "Is Enemy"),
	InBombRange UMETA(DisplayName = "In Bomb Range"),
	InRemoteRange UMETA(DisplayName = "In Remote Range"),
	InAirRange UMETA(DisplayName = "In Air Range"),
};

USTRUCT(BlueprintType)
struct FEnemyCondSet
{
	GENERATED_BODY()
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<EEnemyCond> CondSet;
};

USTRUCT(BlueprintType)
struct FRobotState
{
	GENERATED_BODY()
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FRobotCondSet RobotCond;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FTileCondSet> TileCond;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FEnemyCondSet> EnemyCond;
	FRobotState()
	{
		RobotCond = {};
		TileCond.Init({}, GTotal);
		EnemyCond.Init({}, 4);
	}
	void AddRobotCond(const ERobotCond InCond)
	{
		RobotCond.CondSet.AddUnique(InCond);
	}

	void AddTileCond(const FTile Tile, const ETileCond InCond)
	{
		TileCond[Tile.Index()].CondSet.AddUnique(InCond);
	}

	void AddEnemyCond(const int32 Index, const EEnemyCond InCond)
	{
		EnemyCond[Index].CondSet.AddUnique(InCond);
	}
};

USTRUCT(BlueprintType)
struct FRobotPlan
{
	GENERATED_BODY()
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FRobotState Precondition;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FRobotState EffectRemove;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FRobotState EffectAdd;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FRobotTask> TaskList;

	bool IsSatisfied(const FRobotState& InState) const;
	FRobotState Transform(const FRobotState& InState) const;
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
	static FRobotState QueryState(const AArmedBomber* Bomber);
};
