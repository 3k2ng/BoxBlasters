#pragma once

#include "CoreMinimal.h"
#include "Libraries/CommonUtils.h"

UENUM(BlueprintType)
enum class ETileGen : uint8
{
	Standard UMETA(DisplayName = "Standard"),
	Empty UMETA(DisplayName = "Empty"),
	Random UMETA(DisplayName = "Random"),
};

void GenerateTiles(TArray<ETileType>& TileMap, TArray<ELootType>& LootMap, const ETileGen TileGen);