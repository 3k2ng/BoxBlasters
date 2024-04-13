#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "Components/BomberMovementComponent.h"
#include "GameFramework/Pawn.h"
#include "Actors/Arena.h"
#include "Bomber.generated.h"

UCLASS()
class BOXBLASTERS_API ABomber : public APawn
{
	GENERATED_BODY()
	UPROPERTY()
	UCapsuleComponent* CapsuleComponent;
	UPROPERTY()
	UBomberMovementComponent* MovementComponent;

public:
	ABomber();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	AArena* Arena;

	UPROPERTY(BlueprintReadOnly)
	int32 Index;

	UPROPERTY(BlueprintReadOnly)
	int32 Bombs = 1;

	UPROPERTY(BlueprintReadOnly)
	int32 BombsPlaced = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 Power = 1;

	UPROPERTY(BlueprintReadOnly)
	int32 Speed = 0;

protected:
	virtual void BeginPlay() override;
public:
	UFUNCTION(BlueprintCallable)
	void BombUp();
	UFUNCTION(BlueprintCallable)
	void PowerUp();
	UFUNCTION(BlueprintCallable)
	void SpeedUp();
	UFUNCTION(BlueprintCallable)
	void ConsumeBomb();
	UFUNCTION(BlueprintCallable)
	void ReplenishBomb();
	UFUNCTION(BlueprintPure)
	bool IsBombAvailable() const;
	UFUNCTION(BlueprintPure)
	FTile GetCurrentTile() const;
};
