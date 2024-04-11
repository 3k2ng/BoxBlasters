#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "Components/BomberMovementComponent.h"
#include "GameFramework/Pawn.h"
#include "Actors/Arena.h"
#include "Camera/CameraComponent.h"
#include "Bomber.generated.h"

constexpr float CameraViewWidth = 15.F;
constexpr float CameraFieldOfView = 60.F;
constexpr float CameraAngle = 60.F;

UCLASS()
class BOXBLASTERS_API ABomber : public APawn
{
	GENERATED_BODY()

public:
	ABomber();

	UPROPERTY(EditDefaultsOnly)
	UCapsuleComponent* CapsuleComponent;
	
	UPROPERTY(EditDefaultsOnly)
	UBomberMovementComponent* MovementComponent;

	UPROPERTY(BlueprintReadOnly)
	AArena* Arena;

	UPROPERTY(BlueprintReadOnly)
	int32 Index;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable)
	void SetArena(AArena* InArena);
	UFUNCTION(BlueprintCallable)
	void SetIndex(int32 InIndex);
};
