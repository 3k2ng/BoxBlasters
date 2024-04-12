#include "Actors/Bomb.h"

ABomb::ABomb(): Arena(nullptr), Bomber(nullptr), Power(0)
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABomb::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABomb::SetBomber(ABomber* InBomber)
{
	CHECK_VALID(InBomber);
	Bomber = InBomber;
	Arena = InBomber->Arena;
	Power = InBomber->Power;
}

FTile ABomb::GetCurrentTile() const
{
	return LocationTile(GetActorLocation());
}
