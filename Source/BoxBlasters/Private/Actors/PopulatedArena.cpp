#include "Actors/PopulatedArena.h"

void APopulatedArena::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
	for (int i = 0; i < 4; ++i)
	{
		if (IsValid(Bombers[i]))
		{
			if (ExplosionTimerMap[Bombers[i]->GetCurrentTile().Index()] >= 0.F)
			{
				Bombers[i]->Destroy();
				OnBomberHit(i);
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
	for (int i = 0; i < 4; ++i)
	{
		if (IsValid(BomberClass[i]))
		{
			Bombers[i] = GetWorld()->SpawnActor<AArmedBomber>(
				BomberClass[i],
				FTransform{Spawns[i].Location()}
			);
			Bombers[i]->Arena = this;
		}
	}
}
