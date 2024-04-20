#pragma once

#include "CoreMinimal.h"
#include "CommonUtils.h"
#include "Actors/Bomber.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PlanningUtils.generated.h"

UENUM(BlueprintType)
enum class ERobotPhase : uint8
{
	Decide UMETA(DisplayName = "Decide"),
	Escape UMETA(DisplayName = "Escape"),
	Gather UMETA(DisplayName = "Gather"),
	Eliminate UMETA(DisplayName = "Eliminate"),
};

UCLASS()
class BOXBLASTERS_API UPlanningUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	UFUNCTION(BlueprintPure)
	static TArray<FTile> GetBoxes(const ABomber* Bomber,
	                              const bool FindWhite, const bool FindRed, const bool FindGreen, const bool FindBlue);
	UFUNCTION(BlueprintPure)
	static TArray<FTile> GetPickUps(const ABomber* Bomber,
	                                const bool FindUpgrade, const bool FindSpecial,
	                                const bool FindRed, const bool FindGreen, const bool FindBlue);
	UFUNCTION(BlueprintPure)
	static TArray<FTile> GetOtherBombers(const ABomber* Bomber);
};
