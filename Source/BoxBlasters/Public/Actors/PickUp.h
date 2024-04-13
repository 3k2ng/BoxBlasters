#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickUp.generated.h"

UENUM(BlueprintType)
enum class EPickUpType : uint8
{
	Upgrade UMETA(DisplayName = "Upgrade"),
	Special UMETA(DisplayName = "Special"),
};

UENUM(BlueprintType)
enum class EPickUpColor : uint8
{
	Red UMETA(DisplayName = "Red"),
	Green UMETA(DisplayName = "Green"),
	Blue UMETA(DisplayName = "Blue"),
};

UCLASS()
class BOXBLASTERS_API APickUp : public AActor
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TEnumAsByte<EPickUpType> Type;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TEnumAsByte<EPickUpColor> Color;
};