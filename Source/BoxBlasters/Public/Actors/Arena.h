#pragma once

#include "CoreMinimal.h"
#include "Actors/Area.h"
#include "Libraries/CommonUtils.h"
#include "GameFramework/Actor.h"
#include "Arena.generated.h"

UCLASS()
class BOXBLASTERS_API AArena : public AActor
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere)
	UInstancedStaticMeshComponent* InstancedWall;
	UPROPERTY(EditAnywhere)
	UInstancedStaticMeshComponent* InstancedBox;
	UPROPERTY(EditAnywhere)
	UInstancedStaticMeshComponent* InstancedExplosion;
	
public:	
	AArena();
	UPROPERTY()
	TArray<TEnumAsByte<ETileType>> TileMap;
	UPROPERTY()
	TArray<TEnumAsByte<ELootType>> LootMap;
	UPROPERTY()
	TArray<int32> BoxInstanceMap;
	UPROPERTY()
	TArray<AArea*> AreaObjectMap;
	UPROPERTY()
	TArray<float> ExplosionTimerMap;
	UPROPERTY()
	TArray<TEnumAsByte<EBombType>> BombTypeMap;
	UPROPERTY()
	TArray<int32> BombPowerMap;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AArea> AreaClass;
	UPROPERTY()
	TArray<FTile> AirStrikeTarget;
	UPROPERTY()
	TArray<bool> AirStrikeActive;
	UPROPERTY()
	TArray<int32> AirStrikePower;
protected:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform & Transform) override;
	void AddBox(FTile Tile, ETileType BoxType);
	void UpdateBoxes();
	void UpdateAreaMap();
public:
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable)
	bool PlaceBomb(EBombType BombType, FTile BombTile, int32 BombPower);
	UFUNCTION(BlueprintCallable)
	bool TryBreak(FTile Tile);
	UFUNCTION(BlueprintCallable)
	TArray<FTile> DetonateBomb(FTile BombTile);
	UFUNCTION(BlueprintCallable)
	TArray<FTile> DetonateAirStrike(int32 Index, int32 BombPower);
	UFUNCTION(BlueprintCallable)
	TArray<FTile> BombExplode(FTile BombTile, int32 BombPower);
	UFUNCTION(BlueprintCallable)
	void ExplodeAt(FTile Tile);
	UFUNCTION(BlueprintPure)
	TArray<FTile> GetBombedTiles(FTile BombTile, int32 BombPower);
};
