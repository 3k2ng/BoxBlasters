#pragma once

#include "CoreMinimal.h"
#include "Actors/Arena.h"
#include "Actors/Bomber.h"
#include "PopulatedArena.generated.h"

UCLASS()
class BOXBLASTERS_API APopulatedArena : public AArena
{
	GENERATED_BODY()
	UPROPERTY()
	ABomber* Bombers[4];
public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<ABomber> BomberClasses[4];
protected:
	virtual void BeginPlay() override;
};
