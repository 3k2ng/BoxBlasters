#pragma once

#include "CoreMinimal.h"
#include "Actors/Arena.h"
#include "Actors/Bomber.h"
#include "PopulatedArena.generated.h"

UCLASS()
class BOXBLASTERS_API APopulatedArena : public AArena
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<ABomber> BomberClass[4];
	UPROPERTY(EditAnywhere)
	ABomber* Bombers[4];
protected:
	virtual void BeginPlay() override;
};
