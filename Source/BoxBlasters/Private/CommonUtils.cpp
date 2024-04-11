#include "CommonUtils.h"

int32 FTile::Index() const
{
	return X + GX * Y;
}

FVector FTile::Location() const
{
	UE_LOG(LogTemp, Log, TEXT("%f %f"), GOffsetX, GOffsetY);
	return FVector{
		GTileSize * (static_cast<float>(X) + GOffsetX),
		GTileSize * (static_cast<float>(Y) + GOffsetY),
		0
	};
}
