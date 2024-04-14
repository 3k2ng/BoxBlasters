#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CommonUtils.generated.h"

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

// throw a fatal error if x is not a valid ref
#define CHECK_VALID(x) checkf(IsValid(x), TEXT("invalid ref"))

// all information regarding the size of the arena are hardcoded
// this is to reduce dependency on the arena object
constexpr float GTileSize = 100.F;

// grid size
constexpr int32 GX = 15;
constexpr int32 GY = 13;
constexpr int32 GTotal = GX * GY;

// offset of the (0, 0) tile from the origin of the arena
constexpr float GOffsetX = 0.5F - GX * 0.5F;
constexpr float GOffsetY = 0.5F - GY * 0.5F;

USTRUCT(BlueprintType)
struct FTile
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 X;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Y;
	// is the tile inside the grid
	bool IsValid() const;
	// canonical index of the tile
	int32 Index() const;
	// location of the tile
	FVector Location() const;
	// in the case that a tile is not valid, there are two ways to make it valid
	// modulo its two components with max to shift it back inside the grid
	FTile Mod() const;
	// clamp its two components between 0 and max
	FTile Clamp() const;
	TArray<FTile> Neighbors() const;
	TArray<FTile> InRadius(int32 Radius) const;
};

// return the tile at the canonical index given
inline FTile IndexTile(const int32 Index)
{
	checkf(Index >= 0 && Index < GTotal, TEXT("index must be between 0 and %d"), GTotal);
	return {Index % GX, Index / GX};
}

// return the tile of the location given
inline FTile LocationTile(const FVector Location)
{
	return {
		static_cast<int32>(Location.X / GTileSize - GOffsetX + 0.5F),
		static_cast<int32>(Location.Y / GTileSize - GOffsetY + 0.5F)
	};
}

inline bool operator<(const FTile A, const FTile B)
{
	return A.X < B.X || (A.X == B.X && A.Y < B.Y);
};

inline bool operator==(const FTile A, const FTile B)
{
	return A.X == B.X && A.Y == B.Y;
};

inline bool operator!=(const FTile A, const FTile B)
{
	return A.X != B.X || A.Y != B.Y;
};

inline FTile operator+(const FTile A, const FTile B)
{
	return {A.X + B.X, A.Y + B.Y};
};

inline FTile operator-(const FTile A, const FTile B)
{
	return {A.X - B.X, A.Y - B.Y};
};

inline FTile operator*(const int32 Number, const FTile Tile)
{
	return {Number * Tile.X, Number * Tile.Y};
};

inline FTile operator*(const FTile Tile, const int32 Number)
{
	return {Number * Tile.X, Number * Tile.Y};
}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

constexpr float GBaseBomberSpeed = 4.F * GTileSize;
constexpr float GSpeedMultiplier = 0.25F;

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

UENUM(BlueprintType)
enum class EBombType : uint8
{
	None UMETA(DisplayName = "None"),
	Normal UMETA(DisplayName = "Normal"),
	Remote UMETA(DisplayName = "Remote"),
	Mine UMETA(DisplayName = "Mine"),
};

UENUM(BlueprintType)
enum class ETileType : uint8
{
	Empty UMETA(DisplayName = "Empty"),
	Wall UMETA(DisplayName = "Wall"),
	Basic UMETA(DisplayName = "Basic"),
	Reinforced UMETA(DisplayName = "Reinforced"),
	White UMETA(DisplayName = "White"),
	Red UMETA(DisplayName = "Red"),
	Green UMETA(DisplayName = "Green"),
	Blue UMETA(DisplayName = "Blue"),
};

UENUM(BlueprintType)
enum class ELootType : uint8
{
	None UMETA(DisplayName = "None"),
	Upgrade UMETA(DisplayName = "Upgrade"),
	Special UMETA(DisplayName = "Special"),
};

UCLASS()
class BOXBLASTERS_API UCommonUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	UFUNCTION(BlueprintPure)
	static FVector GetTileLocation(FTile Tile);
	UFUNCTION(BlueprintPure)
	static FTile GetLocationTile(FVector Location);
};
