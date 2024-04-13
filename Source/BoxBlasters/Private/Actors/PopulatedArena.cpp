#include "Actors/PopulatedArena.h"

void APopulatedArena::BeginPlay()
{
	Super::BeginPlay();
	constexpr FTile Spawns[] = {
		{0, 0}, {-1, 0}, {0, -1}, {-1, -1}
	};
	for (int i = 0; i < 4; ++i)
	{
		if (IsValid(BomberClass[i]))
		{
			Bombers[i] = GetWorld()->SpawnActor<ABomber>(
				BomberClass[i],
				FTransform{Spawns[i].Location()}
			);
			Bombers[i]->Arena = this;
		}
	}
}
