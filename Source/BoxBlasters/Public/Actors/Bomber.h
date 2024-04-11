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

public:
	ABomber();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UCapsuleComponent* CapsuleComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UBomberMovementComponent* MovementComponent;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
};
