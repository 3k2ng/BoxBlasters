#include "Actors/AirStrike.h"

FVector ClampAirStrike(const FVector Location)
{
	return ClampVector(
		Location,
		GTileSize * FVector{GOffsetX, GOffsetY, 0.F},
		GTileSize * FVector{-GOffsetX, -GOffsetY, 0.F}
	);
}

void AAirStrike::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IsValid(Arena) && IsValid(Bomber))
	{
		if (Movement.Size() > 1e-3F)
		{
			const float MovementSpeed = 0.667F * GBaseBomberSpeed * (1.F + static_cast<float>(Bomber->Speed) *
				GSpeedMultiplier);
			SetActorLocation(
				ClampAirStrike(GetActorLocation() + Movement.GetClampedToMaxSize(1.0f) * DeltaTime * MovementSpeed));
		}
		Arena->AirStrikeTarget[Bomber->Index] = GetCurrentTile();
		Arena->AirStrikePower[Bomber->Index] = Power;
	}
}
