#pragma once

#include "CoreMinimal.h"
#include "Actors/Bomb.h"
#include "AirStrike.generated.h"

UCLASS()
class BOXBLASTERS_API AAirStrike : public ABomb
{
	GENERATED_BODY()
public:
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FVector Movement;
};
