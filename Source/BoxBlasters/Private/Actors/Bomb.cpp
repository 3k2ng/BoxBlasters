#include "Actors/Bomb.h"

ABomb::ABomb(): Arena(nullptr), Bomber(nullptr)
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
}

FTile ABomb::GetCurrentTile() const
{
	return LocationTile(GetActorLocation());
}
