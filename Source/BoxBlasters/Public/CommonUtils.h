#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CommonUtils.generated.h"

// throw a fatal error if x is not a valid ref
#define CHECK_VALID(x) checkf(IsValid(x), TEXT("invalid ref"))

// all information regarding the size of the arena are hardcoded
// this is do reduce dependency on the arena object
constexpr float GTileSize = 100.F;

// grid size
constexpr int32 GX = 5;
constexpr int32 GY = 5;
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
	bool IsValid() const;
	// canonical index of the tile
	int32 Index() const;
	// location of the tile
	FVector Location() const;
};

// return the tile at the canonical index given
inline FTile IndexTile(const int32 Index)
{
	checkf(Index >= 0 && Index < GTotal, TEXT("index must be between 0 and %d"), GTotal);
	return {Index % GX, Index / GX};
}

UCLASS()
class BOXBLASTERS_API UCommonUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
};
