#include "Libraries/PlanningUtils.h"

#include "Actors/PopulatedArena.h"
#include "Kismet/GameplayStatics.h"

TArray<FTile> UPlanningUtils::GetBoxes(
	const ABomber* Bomber,
	const bool FindWhite,
	const bool FindRed,
	const bool FindGreen,
	const bool FindBlue)
{
	CHECK_VALID(Bomber);
	const APopulatedArena* Arena = Cast<APopulatedArena>(Bomber->Arena);
	CHECK_VALID(Arena);
	TArray<FTile> BoxTiles;
	for (int i = 0; i < GTotal; ++i)
	{
		if ((FindWhite && Arena->TileMap[i] == ETileType::White) ||
			(FindRed && Arena->TileMap[i] == ETileType::Red) ||
			(FindGreen && Arena->TileMap[i] == ETileType::Green) ||
			(FindBlue && Arena->TileMap[i] == ETileType::Blue)
		)
			BoxTiles.Add(IndexTile(i));
	}
	return BoxTiles;
}

TArray<FTile> UPlanningUtils::GetPickUps(
	const ABomber* Bomber,
	const bool FindUpgrade,
	const bool FindSpecial,
	const bool FindRed,
	const bool FindGreen,
	const bool FindBlue)
{
	CHECK_VALID(Bomber);
	const APopulatedArena* Arena = Cast<APopulatedArena>(Bomber->Arena);
	CHECK_VALID(Arena);
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(Bomber->GetWorld(), APickUp::StaticClass(), FoundActors);
	TArray<FTile> FoundPickUps;
	for (const AActor* FoundActor : FoundActors)
	{
		const APickUp* PickUp = Cast<APickUp>(FoundActor);
		if (
			((FindUpgrade && PickUp->Type == EPickUpType::Upgrade) ||
				(FindSpecial && PickUp->Type == EPickUpType::Special)) &&
			((FindRed && PickUp->Color == EPickUpColor::Red) ||
				(FindGreen && PickUp->Color == EPickUpColor::Green) ||
				(FindBlue && PickUp->Color == EPickUpColor::Blue))
		)
		{
			FoundPickUps.Add(PickUp->GetCurrentTile());
		}
	}
	return FoundPickUps;
}

TArray<FTile> UPlanningUtils::GetOtherBombers(const ABomber* Bomber)
{
	CHECK_VALID(Bomber);
	const APopulatedArena* Arena = Cast<APopulatedArena>(Bomber->Arena);
	CHECK_VALID(Arena);
	TArray<FTile> OtherBombers;
	for (int i = 0; i < 4; ++i)
	{
		if (i != Bomber->Index && IsValid(Arena->Bombers[i]))
			OtherBombers.Add(Arena->Bombers[i]->CurrentTile);
	}
	return OtherBombers;
}
