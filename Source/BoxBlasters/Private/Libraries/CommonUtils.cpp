#include "Libraries/CommonUtils.h"

bool FTile::IsValid() const
{
	return 0 <= X && X < GX && 0 <= Y && Y < GY;
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

FTile FTile::Mod() const
{
	return {
		((X % GX) + GX) % GX,
		((Y % GY) + GY) % GY
	};
}

FTile FTile::Clamp() const
{
	return {
		std::max(0, std::min(GX, X)),
		std::max(0, std::min(GY, Y))
	};
}

TArray<FTile> FTile::InRadius(const int32 Radius) const
{
	TArray<FTile> Tiles;
	for (int i = 0; i < GTotal; ++i)
	{
		const FTile Dist = IndexTile(i) - *this;
		if (Dist.X * Dist.X + Dist.Y * Dist.Y < Radius * Radius) Tiles.Add(IndexTile(i));
	}
	return Tiles;
}

FVector UCommonUtils::GetTileLocation(const FTile Tile)
{
	return Tile.Location();
}

FTile UCommonUtils::GetLocationTile(const FVector Location)
{
	return LocationTile(Location);
}
