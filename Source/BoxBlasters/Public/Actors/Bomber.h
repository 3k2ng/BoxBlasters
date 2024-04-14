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

	UPROPERTY(BlueprintReadOnly)
	AArena* Arena;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 Index;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 Bombs = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 BombsPlaced = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 Power = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 Speed = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FTile CurrentTile;

protected:
	virtual void BeginPlay() override;
public:
	virtual void Tick(float DeltaTime) override;
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
};
