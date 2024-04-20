#include "Libraries/TileGen.h"

void GenerateTiles(TArray<ETileType>& TileMap, TArray<ELootType>& LootMap, const ETileGen TileGen)
{
	TArray<FTile> OccupiedTiles = {
		{0, 0}, {-2, 0}, {0, -2}, {-2, -2}
	};
	for (int i = 0; i < 4; ++i)
	{
		OccupiedTiles[i] = OccupiedTiles[i].Mod();
		OccupiedTiles.Add(OccupiedTiles[i] + FTile{0, 1});
		OccupiedTiles.Add(OccupiedTiles[i] + FTile{1, 0});
		OccupiedTiles.Add(OccupiedTiles[i] + FTile{1, 1});
	}
	switch (TileGen)
	{
	case ETileGen::Standard:
		{
			for (int i = 0; i < GTotal; ++i)
			{
				const FTile Tile = IndexTile(i);
				if (!OccupiedTiles.Contains(Tile) && TileMap[i] != ETileType::Wall)
				{
					TileMap[i] = ETileType::Basic;
					if ((Tile.X == 3 || Tile.X == 11) && Tile.Y % 2 == 0 && (Tile.Y < 4 || Tile.Y > 8))
						TileMap[i] =
							ETileType::Reinforced;
					if ((Tile.Y == 3 || Tile.Y == 9) && Tile.X % 2 == 0 && (Tile.X < 4 || Tile.X > 10))
						TileMap[i] =
							ETileType::Reinforced;
					if ((Tile.X == 5 || Tile.X == 9) && Tile.Y % 2 == 0 && (Tile.Y < 6 || Tile.Y > 6))
						TileMap[i] =
							ETileType::Reinforced;
					if ((Tile.Y == 5 || Tile.Y == 7) && Tile.X % 2 == 0 && (Tile.X < 6 || Tile.X > 8))
						TileMap[i] =
							ETileType::Reinforced;
				}
			}
			constexpr ETileType Colors[] = {ETileType::Red, ETileType::Green, ETileType::Blue};
			int QuarterUpgrades[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
			for (int i = 0; i < 12; ++i)
			{
				while (true)
				{
					const int32 RandX = FMath::RandRange(0, GX - 1);
					const int32 RandY = FMath::RandRange(0, GY - 1);
					if (((RandX >= 4 && RandX < GX - 4) || (RandY >= 4 && RandY < GY - 4)) &&
						TileMap[FTile{RandX, RandY}.Index()] == ETileType::Basic)
					{
						constexpr int UpgradesEachQuarter = 1;
						const bool IsLeftHalf = RandX <= GX / 2;
						const bool IsLowerHalf = RandY <= GY / 2;
						if (IsLeftHalf && IsLowerHalf)
						{
							if (QuarterUpgrades[0 + i % 3] >= UpgradesEachQuarter) continue;
							++QuarterUpgrades[0 + i % 3];
						}
						else if (!IsLeftHalf && IsLowerHalf)
						{
							if (QuarterUpgrades[3 + i % 3] >= UpgradesEachQuarter) continue;
							++QuarterUpgrades[3 + i % 3];
						}
						else if (IsLeftHalf && !IsLowerHalf)
						{
							if (QuarterUpgrades[6 + i % 3] >= UpgradesEachQuarter) continue;
							++QuarterUpgrades[6 + i % 3];
						}
						else
						{
							if (QuarterUpgrades[9 + i % 3] >= UpgradesEachQuarter) continue;
							++QuarterUpgrades[9 + i % 3];
						}
						TileMap[FTile{RandX, RandY}.Index()] = Colors[i % 3];
						LootMap[FTile{RandX, RandY}.Index()] = ELootType::Upgrade;
						break;
					}
				}
			}
			for (int i = 0; i < 6; ++i)
			{
				while (true)
				{
					const int32 RandX = FMath::RandRange(0, GX - 1);
					const int32 RandY = FMath::RandRange(0, GY - 1);
					if (((RandX >= 4 && RandX < GX - 4) || (RandY >= 4 && RandY < GY - 4)) &&
						TileMap[FTile{RandX, RandY}.Index()] == ETileType::Basic)
					{
						TileMap[FTile{RandX, RandY}.Index()] = Colors[i % 3];
						LootMap[FTile{RandX, RandY}.Index()] = ELootType::Special;
						break;
					}
				}
			}
			int QuarterWhiteDrops[] = {0, 0, 0, 0};
			for (int i = 0; i < 12; ++i)
			{
				while (true)
				{
					const int32 RandX = FMath::RandRange(0, GX - 1);
					const int32 RandY = FMath::RandRange(0, GY - 1);
					if (((RandX >= 4 && RandX < GX - 4) || (RandY >= 4 && RandY < GY - 4)) &&
						TileMap[FTile{RandX, RandY}.Index()] == ETileType::Basic)
					{
						const bool IsLeftHalf = RandX <= GX / 2;
						const bool IsLowerHalf = RandY <= GY / 2;
						constexpr int WhiteDropsEachQuarter = 3;
						if (IsLeftHalf && IsLowerHalf)
						{
							if (QuarterWhiteDrops[0] >= WhiteDropsEachQuarter) continue;
							++QuarterWhiteDrops[0];
						}
						else if (!IsLeftHalf && IsLowerHalf)
						{
							if (QuarterWhiteDrops[1] >= WhiteDropsEachQuarter) continue;
							++QuarterWhiteDrops[1];
						}
						else if (IsLeftHalf && !IsLowerHalf)
						{
							if (QuarterWhiteDrops[2] >= WhiteDropsEachQuarter) continue;
							++QuarterWhiteDrops[2];
						}
						else
						{
							if (QuarterWhiteDrops[3] >= WhiteDropsEachQuarter) continue;
							++QuarterWhiteDrops[3];
						}
						TileMap[FTile{RandX, RandY}.Index()] = ETileType::White;
						LootMap[FTile{RandX, RandY}.Index()] = ELootType::Upgrade;
						break;
					}
				}
			}
			for (int i = 0; i < GTotal; ++i)
			{
				if (TileMap[i] == ETileType::Basic && FMath::RandRange(1, 100) <= 50) TileMap[i] = ETileType::White;
			}
		}
		break;
	case ETileGen::Empty:
		break;
	case ETileGen::Random:
		{
			for (int i = 0; i < GTotal; ++i)
			{
				if (!OccupiedTiles.Contains(IndexTile(i)) && TileMap[i] != ETileType::Wall)
				{
					switch (FMath::RandRange(0, 9))
					{
					case 0: TileMap[i] = ETileType::Basic;
						LootMap[i] = ELootType::None;
						break;
					case 1: TileMap[i] = ETileType::Reinforced;
						LootMap[i] = ELootType::None;
						break;
					case 2: TileMap[i] = ETileType::White;
						LootMap[i] = ELootType::None;
						break;
					case 3: TileMap[i] = ETileType::White;
						LootMap[i] = ELootType::Upgrade;
						break;
					case 4: TileMap[i] = ETileType::Red;
						LootMap[i] = ELootType::Upgrade;
						break;
					case 5: TileMap[i] = ETileType::Red;
						LootMap[i] = ELootType::Special;
						break;
					case 6: TileMap[i] = ETileType::Green;
						LootMap[i] = ELootType::Upgrade;
						break;
					case 7: TileMap[i] = ETileType::Green;
						LootMap[i] = ELootType::Special;
						break;
					case 8: TileMap[i] = ETileType::Blue;
						LootMap[i] = ELootType::Upgrade;
						break;
					case 9: TileMap[i] = ETileType::Blue;
						LootMap[i] = ELootType::Special;
						break;
					default: ;
					}
				}
			}
		}
		break;
	case ETileGen::Maze:
		for (int i = 0; i < GTotal; ++i)
		{
			const FTile Tile = IndexTile(i);
			if (!OccupiedTiles.Contains(Tile) && TileMap[i] != ETileType::Wall &&
				IndexTile(i).X != 0 && IndexTile(i).Y != 6 && IndexTile(i).X != 4 && IndexTile(i).X != 8 && IndexTile(i).X != 12)
			{
				TileMap[i] = ETileType::Basic;
			}
		}
		break;
	case ETileGen::BoxInTheMiddle:
		for (int i = 0; i < GTotal; ++i)
		{
			const FTile Tile = IndexTile(i);
			if (!OccupiedTiles.Contains(Tile) && TileMap[i] != ETileType::Wall)
			{
				TileMap[i] = ETileType::Basic;
			}
			TileMap[FTile{4, 4}.Index()] = ETileType::Red;
			LootMap[FTile{4, 4}.Index()] = ELootType::Upgrade;
		}
		break;
	}
}
