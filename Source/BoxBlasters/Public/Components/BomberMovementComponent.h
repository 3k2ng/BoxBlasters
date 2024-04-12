#pragma once

#include "CoreMinimal.h"
#include "Libraries/CommonUtils.h"
#include "GameFramework/PawnMovementComponent.h"
#include "BomberMovementComponent.generated.h"

UCLASS()
class BOXBLASTERS_API UBomberMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()
	UPROPERTY()
	float MovementSpeed = GBaseBomberSpeed;
public:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	void UpdateSpeed(int32 InSpeed);
};
