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
}
