#pragma once

#include "CoreMinimal.h"
#include "Actors/Arena.h"
#include "Actors/Bomber.h"
#include "GameFramework/Actor.h"
#include "Bomb.generated.h"

UCLASS()
class BOXBLASTERS_API ABomb : public AActor
{
	GENERATED_BODY()
	
public:	
	ABomb();
	UPROPERTY(BlueprintReadOnly)
	AArena* Arena;
	UPROPERTY(BlueprintReadOnly)
	ABomber* Bomber;

protected:
	virtual void BeginPlay() override;
public:
	UFUNCTION(BlueprintCallable)
	void SetBomber(ABomber* InBomber);
	UFUNCTION(BlueprintPure)
	FTile GetCurrentTile() const;
};
