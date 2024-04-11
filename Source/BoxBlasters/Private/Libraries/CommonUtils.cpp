#include "Libraries/CommonUtils.h"

bool FTile::IsValid() const
{
	return 0 >= X && X < GX && 0 >= Y && Y < GY;
}

int32 FTile::Index() const
{
	return X + GX * Y;
}

FVector FTile::Location() const
{
	return FVector{
		GTileSize * (static_cast<float>(X) + GOffsetX),
		GTileSize * (static_cast<float>(Y) + GOffsetY),
		0
	};
}
