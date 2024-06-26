#pragma once

#include "CoreMinimal.h"
#include "Actors/PickUp.h"
#include "Libraries/CommonUtils.h"
#include "Libraries/TileGen.h"
#include "GameFramework/Actor.h"
#include "Arena.generated.h"

UCLASS()
class BOXBLASTERS_API AArena : public AActor
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly)
	UInstancedStaticMeshComponent* InstancedWall;
	UPROPERTY(EditDefaultsOnly)
	UInstancedStaticMeshComponent* InstancedBox;
	UPROPERTY(EditDefaultsOnly)
	UInstancedStaticMeshComponent* InstancedWarning;
	UPROPERTY(EditDefaultsOnly)
	UInstancedStaticMeshComponent* InstancedExplosion;
	UPROPERTY(EditAnywhere)
	ETileGen TileGen;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<APickUp> RedUpgrade;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<APickUp> GreenUpgrade;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<APickUp> BlueUpgrade;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<APickUp> RedSpecial;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<APickUp> GreenSpecial;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<APickUp> BlueSpecial;
public:	
	AArena();
	UPROPERTY()
	TArray<ETileType> TileMap;
	UPROPERTY()
	TArray<ELootType> LootMap;
	UPROPERTY()
	TArray<int32> BoxInstanceMap;
	UPROPERTY()
	TArray<bool> DangerMap;
	UPROPERTY()
	TArray<float> ExplosionTimerMap;
	UPROPERTY()
	TArray<EBombType> BombTypeMap;
	UPROPERTY()
	TArray<int32> BombPowerMap;
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
	void UpdateWarning();
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
	TArray<FTile> GetBombedTiles(FTile BombTile, int32 BombPower) const;
};
