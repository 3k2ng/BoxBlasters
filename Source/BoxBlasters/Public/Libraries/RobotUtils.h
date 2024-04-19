#pragma once

#include "CoreMinimal.h"
#include "CommonUtils.h"
#include "Actors/Bomber.h"
#include "Actors/PopulatedArena.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RobotUtils.generated.h"

USTRUCT(BlueprintType)
struct FMaybeTile
{
	GENERATED_BODY()
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool HasTile;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FTile JustTile;

	static FMaybeTile None() { return {false, {}}; }
	static FMaybeTile Just(const FTile InTarget) { return {true, InTarget}; }
};

UENUM(BlueprintType)
enum class ERobotTaskType : uint8
{
	Move UMETA(DisplayName = "Move"),
	Wait UMETA(DisplayName = "Wait"),
	Strike UMETA(DisplayName = "Strike"),
	Bomb UMETA(DisplayName = "Bomb"),
	Special UMETA(DisplayName = "Special"),
};

USTRUCT(BlueprintType)
struct FRobotTask
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ERobotTaskType TaskType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTile Target;
};

UENUM(BlueprintType)
enum class ETileStatus : uint8
{
	Safe UMETA(DisplayName = "Safe"),
	Danger UMETA(DisplayName = "Danger"),
	Bomb UMETA(DisplayName = "Bomb"),
	Wall UMETA(DisplayName = "Wall"),
	Breakable UMETA(DisplayName = "Breakable"),
	Reinforced UMETA(DisplayName = "Reinforced"),
};

struct FStatusMap
{
	TArray<ETileStatus> TileStatusMap;
	TArray<int32> BombPowerMap;
	FMaybeTile BomberTile[4];
	explicit FStatusMap(const ABomber* Bomber);
	FStatusMap() { for (int i = 0; i < 4; ++i) BomberTile[i] = FMaybeTile::None(); }
	bool IsSafe(const FTile A) const;
	bool IsBlocked(const FTile A) const;
	TArray<FTile> ReachableTiles(const FTile A, const bool Safe) const;
	int32 WalkCost(const FTile A, const FTile B, const int32 SafeCost, const int32 DangerCost) const;
	TArray<FTile> BombImpact(const FTile BombTile, const int32 BombPower) const;
	FStatusMap BreakTiles(const TArray<FTile>& ToBreak) const;
	FStatusMap PlaceBomb(const FTile BombTile, const int32 BombPower) const;
	FStatusMap DetonateBomb(const FTile A) const;
	TArray<FTile> ReachableSafe(const FTile A, const bool Safe) const;
	TArray<FTile> ViableBombSpots(const FTile A, const int32 BombPower) const;
	TArray<FTile> AStar(const FTile A, const FTile B, const int32 SafeCost, const int32 DangerCost, const int32 Weight) const;
	TArray<FTile> BombAStar(const FTile A, const FTile B, const int32 BombPower, const int32 BombCost, const int32 Weight) const;
};

UCLASS(BlueprintType)
class BOXBLASTERS_API URobotUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure)
	static TArray<FTile> GetAStar(const ABomber* Bomber, const FTile Goal, const int32 SafeCost, const int32 DangerCost, const int32 Weight);
	UFUNCTION(BlueprintPure)
	static TArray<FTile> GetBombAStar(const ABomber* Bomber, const FTile Goal, const int32 BombPower, const int32 BombCost, const int32 Weight);
	UFUNCTION(BlueprintPure)
	static FMaybeTile GetAStarTop(const ABomber* Bomber, const FTile Goal, const int32 SafeCost, const int32 DangerCost, const int32 Weight);
	UFUNCTION(BlueprintPure)
	static FMaybeTile GetBombAStarTop(const ABomber* Bomber, const FTile Goal, const int32 BombPower, const int32 BombCost, const int32 Weight);
	UFUNCTION(BlueprintPure)
	static bool IsTileSafe(const ABomber* Bomber, const FTile A);
	UFUNCTION(BlueprintPure)
	static bool IsTileBlocked(const ABomber* Bomber, const FTile A);
	UFUNCTION(BlueprintPure)
	static bool IsTileReachable(const ABomber* Bomber, const FTile A, const bool Safe);
};
