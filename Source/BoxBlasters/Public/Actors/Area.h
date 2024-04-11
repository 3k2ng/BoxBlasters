#pragma once

#include "CoreMinimal.h"
#include "Area.generated.h"

UENUM(BlueprintType)
enum class EAreaState : uint8
{
	Normal UMETA(DisplayName = "Normal"),
	Warning UMETA(DisplayName = "Warning"),
	Blocked UMETA(DisplayName = "Blocked")
};

UCLASS()
class BOXBLASTERS_API AArea : public AActor
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void OnStateChanged(EAreaState NewState);
};
