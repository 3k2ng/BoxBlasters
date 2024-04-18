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
enum class ETileState : uint8
{
	Normal UMETA(DisplayName = "Normal"),
	Warning UMETA(DisplayName = "Warning"),
	Blocked UMETA(DisplayName = "Blocked"),
};

TArray<ETileState> RequestTileState(const AArena* Arena);
TArray<FTile> RequestReachableTiles(const TArray<ETileState>& TileStateMap, const FTile From, const bool Safe);
TArray<FTile> RequestBombEscapes(const TArray<ETileState>& TileStateMap, const FTile From);
int32 RequestTileCost(const TArray<ETileState>& TileStateMap, const FTile From, const FTile To,
					  const int32 NormalCost, const int32 WarningCost);
FMaybeTile RequestLeastCostTile(const TArray<ETileState>& TileStateMap, const FTile From, const TArray<FTile>& To,
							 const int32 NormalCost, const int32 WarningCost);

struct FPathFindStep
{
	int32 Cost;
	FTile Target;
	FTile Origin;

	FPathFindStep(const int32 InCost,
				  const FTile InTarget,
				  const FTile InOrigin) : Cost(InCost), Target(InTarget), Origin(InOrigin) {}
};

inline bool operator<(FPathFindStep const& A, FPathFindStep const& B)
{
	return A.Cost > B.Cost;
}

UCLASS(BlueprintType)
class BOXBLASTERS_API URobotUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure)
	static FMaybeTile Dijkstra(const APopulatedArena* Arena, const FTile From, const FTile To, const int32 NormalCost, const int32 WarningCost);
	UFUNCTION(BlueprintPure)
	static bool IsWarningAt(const AArena* Arena, const FTile Tile);
	UFUNCTION(BlueprintPure)
	static bool IsBlockedAt(const AArena* Arena, const FTile Tile);
	UFUNCTION(BlueprintPure)
	static int32 GetDistance(const FTile From, const FTile To);
	UFUNCTION(BlueprintPure)
	static TArray<FTile> FindBoxes(const AArena* Arena, const bool IncludeWhite, const bool IncludeRed, const bool IncludeGreen, const bool IncludeBlue);
	UFUNCTION(BlueprintPure)
	static EBombType GetBombType(const AArena* Arena, const FTile Target);
	UFUNCTION(BlueprintPure)
	static TArray<FTile> GetReachableTiles(const APopulatedArena* Arena, const FTile From, const bool Safe);
	UFUNCTION(BlueprintPure)
	static FMaybeTile GetNearestTile(const FTile From, const TArray<FTile>& To);
	UFUNCTION(BlueprintPure)
	static TArray<FTile> BombSpotsToHit(const ABomber* Bomber, const FTile Target);
	UFUNCTION(BlueprintPure)
	static TArray<FTile> GetPotentialBombEscapes(const ABomber* Bomber, const FTile Target);
	UFUNCTION(BlueprintPure)
	static TArray<FTile> GetCurrentBombEscapes(const ABomber* Bomber);
	UFUNCTION(BlueprintPure)
	static bool CanReachTile(const ABomber* Bomber, const FTile Target, const bool Safe);
};
