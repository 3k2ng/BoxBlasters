#pragma once

#include "CoreMinimal.h"
#include "Actors/Arena.h"
#include "Actors/ArmedBomber.h"
#include "PopulatedArena.generated.h"

UCLASS()
class BOXBLASTERS_API APopulatedArena : public AArena
{
	GENERATED_BODY()
public:
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(EditAnywhere)
	TSubclassOf<ABomber> BomberClass[4];
	UPROPERTY(VisibleAnywhere)
	AArmedBomber* Bombers[4];
protected:
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintPure)
	bool TileHasBomber(FTile Tile, int32 Index) const;
	UFUNCTION(BlueprintPure)
	bool TileHasOtherBomber(FTile Tile, int32 Index) const;
	UFUNCTION(BlueprintImplementableEvent)
	void OnBomberHit(int32 Index);
};
