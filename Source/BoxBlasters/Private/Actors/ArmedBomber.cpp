#include "Actors/ArmedBomber.h"

void AArmedBomber::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IsValid(Arena))
	{
		if (IsValid(DeployedAirStrike))Arena->AirStrikeActive[Index] = true;
		else Arena->AirStrikeActive[Index] = false;
	}
}

bool AArmedBomber::PlaceBomb()
{
	CHECK_VALID(Arena);
	CHECK_VALID(NormalClass);
	if (IsBombAvailable() && Arena->PlaceBomb(EBombType::Normal, GetCurrentTile(), Power))
	{
		const auto PlacedBomb = GetWorld()->SpawnActor<ABomb>(
			NormalClass,
			FTransform{GetCurrentTile().Location()}
		);
		PlacedBomb->SetBomber(this);
		ConsumeBomb();
		return true;
	}
	return false;
}

bool AArmedBomber::ActivateSpecial()
{
	CHECK_VALID(Arena);
	CHECK_VALID(RemoteClass);
	CHECK_VALID(MineClass);
	CHECK_VALID(AirClass);
	if (IsValid(DeployedRemote))
	{
		DeployedRemote->Detonate();
		return false;
	}
	if (IsValid(DeployedAirStrike)) return false;
	switch (EquippedSpecial)
	{
	case ESpecialType::None:
		break;
	case ESpecialType::Remote:
		if (Arena->PlaceBomb(EBombType::Remote, GetCurrentTile(), Power))
		{
			const auto PlacedBomb = GetWorld()->SpawnActor<ABomb>(
				RemoteClass,
				FTransform{GetCurrentTile().Location()}
			);
			PlacedBomb->SetBomber(this);
			EquippedSpecial = ESpecialType::None;
			DeployedRemote = PlacedBomb;
			return true;
		}
		break;
	case ESpecialType::Mine:
		if (Arena->PlaceBomb(EBombType::Mine, GetCurrentTile(), Power))
		{
			const auto PlacedBomb = GetWorld()->SpawnActor<ABomb>(
				MineClass,
				FTransform{GetCurrentTile().Location()}
			);
			PlacedBomb->SetBomber(this);
			EquippedSpecial = ESpecialType::None;
			return true;
		}
		break;
	case ESpecialType::Air:
		{
			const auto PlacedBomb = GetWorld()->SpawnActor<AAirStrike>(
				AirClass,
				FTransform{GetCurrentTile().Location()}
			);
			PlacedBomb->SetBomber(this);
			EquippedSpecial = ESpecialType::None;
			DeployedAirStrike = PlacedBomb;
			return true;
		}
	default: ;
	}
	return false;
}

bool AArmedBomber::EquipSpecial(const ESpecialType SpecialType)
{
	if (EquippedSpecial == ESpecialType::None)
	{
		EquippedSpecial = SpecialType;
		return true;
	}
	return false;
}

bool AArmedBomber::MoveSpecial(const FVector Movement)
{
	if (IsValid(DeployedAirStrike))
	{
		DeployedAirStrike->Movement = Movement;
		return true;
	}
	return false;
}
