#include "Actors/Bomber.h"

ABomber::ABomber()
{
	PrimaryActorTick.bCanEverTick = true;
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>("CollisionCapsule");
	SetRootComponent(CapsuleComponent);
	CapsuleComponent->InitCapsuleSize(40, 50);
	CapsuleComponent->SetCollisionProfileName("Pawn");
	CapsuleComponent->bDynamicObstacle = true;
	CapsuleComponent->bFillCollisionUnderneathForNavmesh = true;
	MovementComponent = CreateDefaultSubobject<UBomberMovementComponent>("CharMoveComp");
}

void ABomber::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABomber::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
	CurrentTile = LocationTile(GetActorLocation());
}

void ABomber::BombUp()
{
	if (!IsValid(this)) return;
	++Bombs;
}

void ABomber::PowerUp()
{
	if (!IsValid(this)) return;
	++Power;
}

void ABomber::SpeedUp()
{
	if (!IsValid(this)) return;
	++Speed;
	MovementComponent->UpdateSpeed(Speed);
}

void ABomber::ConsumeBomb()
{
	if (!IsValid(this)) return;
	++BombsPlaced;
}

void ABomber::ReplenishBomb()
{
	if (!IsValid(this)) return;
	--BombsPlaced;
}

bool ABomber::IsBombAvailable() const
{
	if (!IsValid(this)) return false;
	return Bombs > BombsPlaced;
}
