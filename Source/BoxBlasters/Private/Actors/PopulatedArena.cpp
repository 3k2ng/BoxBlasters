#include "Actors/PopulatedArena.h"

void APopulatedArena::BeginPlay()
{
	Super::BeginPlay();
	constexpr FTile BomberSpawns[] = {
		{0, 0},
		{-1, 0},
		{0, -1},
		{-1, -1}
	};
	for (int i = 0; i < 4; ++i)
	{
		if (IsValid(BomberClasses[i]))
		{
			Bombers[i] = GetWorld()->SpawnActor<ABomber>(
				BomberClasses[i],
				FTransform{BomberSpawns[i].Location()}
			);
			Bombers[i]->Arena = this;
			Bombers[i]->Index = i;
		}
	}
}
