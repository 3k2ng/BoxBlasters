#include "Actors/PopulatedArena.h"

void APopulatedArena::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
	for (int i = 0; i < 4; ++i)
	{
		if (IsValid(Bombers[i]))
		{
			if (ExplosionTimerMap[Bombers[i]->CurrentTile.Index()] >= 0.F)
			{
				Bombers[i]->Destroy();
			}
		}
	}
}

void APopulatedArena::BeginPlay()
{
	Super::BeginPlay();
	constexpr FTile Spawns[] = {
		{0, 0}, {-1, 0}, {0, -1}, {-1, -1}
	};
	for (int32 i = 0; i < 4; ++i)
	{
		if (IsValid(BomberClass[i]))
		{
			Bombers[i] = GetWorld()->SpawnActor<AArmedBomber>(
				BomberClass[i],
				FTransform{Spawns[i].Mod().Location()}
			);
			if (IsValid(Bombers[i]))
			{
				Bombers[i]->Arena = this;
				Bombers[i]->Index = i;
				Bombers[i]->CurrentTile = Spawns[i].Mod();
			}
		}
	}
}
