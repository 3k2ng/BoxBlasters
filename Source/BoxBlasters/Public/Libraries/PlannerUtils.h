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

USTRUCT(BlueprintType)
struct FMapState
{
	GENERATED_BODY()
	UPROPERTY()
	TArray<ETileType> TileMap;
	UPROPERTY()
	TArray<EBombType> BombTypeMap;
	UPROPERTY()
	TArray<int32> BombPowerMap;
	FMapState() {};
	explicit FMapState(const AArmedBomber* Bomber);
	TArray<ETileState> TileState() const;
	TArray<FTile> GetBombedTiles(FTile BombTile, int32 BombPower) const;
	FMapState DamageTiles(TArray<FTile> ToDamage) const;
	FMapState PlaceBomb(FTile BombTile, EBombType BombType, int32 BombPower) const;
	FMapState DetonateBomb(FTile Target) const;
};

struct FBombingPathFindStep
{
	int32 Cost;
	FTile Location;
	FMapState MapState;
	TArray<FRobotTask> TaskList;
};

UCLASS()
class BOXBLASTERS_API UPlannerUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	UFUNCTION(BlueprintPure)
	static FRobotPlan PlanToReach(const AArmedBomber* Bomber, const FTile Target);
};
