#include "Bomber.h"

ABomber::ABomber()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABomber::BeginPlay()
{
	Super::BeginPlay();
}

void ABomber::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABomber::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
