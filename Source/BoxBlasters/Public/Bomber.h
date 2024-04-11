#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Bomber.generated.h"

UCLASS()
class BOXBLASTERS_API ABomber : public APawn
{
	GENERATED_BODY()

public:
	ABomber();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
