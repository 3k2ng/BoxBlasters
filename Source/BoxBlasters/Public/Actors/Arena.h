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
	TArray<TEnumAsByte<EAreaState>> AreaStateMap;
	UPROPERTY()
	TArray<float> ExplosionTimerMap;
	UPROPERTY()
	TArray<bool> ExplosionMap;
	UPROPERTY()
	TArray<TEnumAsByte<EBombType>> BombTypeMap;
	UPROPERTY()
	TArray<int32> BombPowerMap;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AArea> AreaClass;
protected:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform & Transform) override;
	void AddBox(FTile Tile, ETileType BoxType);
	void UpdateBoxes();
public:
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable)
	void PlaceBomb(EBombType BombType, FTile BombTile, int32 BombPower);
	UFUNCTION(BlueprintCallable)
	void DetonateBomb(FTile BombTile);
	UFUNCTION(BlueprintCallable)
	void Explode(FTile Tile);
	UFUNCTION(BlueprintPure)
	TArray<FTile> GetBombedTiles(FTile BombTile, int32 BombPower);
};
