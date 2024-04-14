#pragma once

#include "CoreMinimal.h"
#include "CommonUtils.h"
#include "Actors/Bomber.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RobotUtils.generated.h"

UENUM(BlueprintType)
enum class ERobotTaskType : uint8
{
	Move UMETA(DisplayName = "Move"),
	MoveSpecial UMETA(DisplayName = "Move Special"),
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

UENUM(BlueprintType)
enum class ETileInfo : uint8
{
	Empty UMETA(DisplayName = "Empty"),
	Wall UMETA(DisplayName = "Wall"),
	Basic UMETA(DisplayName = "Basic"),
	Reinforced UMETA(DisplayName = "Reinforced"),
	White UMETA(DisplayName = "White"),
	Red UMETA(DisplayName = "Red"),
	Green UMETA(DisplayName = "Green"),
	Blue UMETA(DisplayName = "Blue"),
	NormalBomb UMETA(DisplayName = "Normal Bomb"),
	RemoteBomb UMETA(DisplayName = "Remote Bomb"),
	Mine UMETA(DisplayName = "Mine"),
};

UCLASS(BlueprintType)
class BOXBLASTERS_API URobotUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	UFUNCTION(BlueprintPure)
	static TArray<FTile> Dijkstra(const ABomber* Bomber, const FTile From, const FTile To, const int32 NormalCost, const int32 WarningCost);
};
