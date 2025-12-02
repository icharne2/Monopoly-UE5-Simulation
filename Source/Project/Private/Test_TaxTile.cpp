#include "Test_TaxTile.h"
#include "Misc/AutomationTest.h"
#include "PlayerCharacter.h"
#include "ABoardTile.h"

// Test 1: Checks if the player pays tax correctly when they enter the tax space.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTaxTilePaymentTest, "Game.Tax.Pay", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FTaxTilePaymentTest::RunTest(const FString& Parameters)
{
    APlayerCharacter* Player = NewObject<APlayerCharacter>();
    Player->Money = 300;

    AABoardTile* TaxTile = NewObject<AABoardTile>();
    TaxTile->TileID = 6;
    TaxTile->TileCost = 100;  

    Player->RemoveMoney(TaxTile->TileCost);

    TestEqual("Tax removed correctly", Player->Money, 200);
    TestFalse("Player is not bankrupt", Player->bIsBankrupt);

    return true;
}

//Test 2: Checks the calculation of the rent premium for having a full set of type 1 fields.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSpecialBonusType1Test, "Game.SpecialRent.Type1", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FSpecialBonusType1Test::RunTest(const FString& Parameters)
{
    APlayerCharacter* Player = NewObject<APlayerCharacter>();

    // 2 fields of type 1 - no bonus 1.0f
    for (int i = 0; i < 2; i++) {
        AABoardTile* Tile = NewObject<AABoardTile>();
        Tile->SpecialType = 1;
        Player->OwnedTiles.Add(Tile);
    }

    Player->UpdateRentBonus();
    TestEqual("Less than 3 type1 gives no bonus", Player->RentBonusType1, 1.0f);

    return true;
}

//Test 3: Checks the increased rent for type 2 fields depending on the number of fields of that type owned.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSpecialBonusType2RentTest, "Game.SpecialRent.Type2", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FSpecialBonusType2RentTest::RunTest(const FString& Parameters)
{
    APlayerCharacter* Player = NewObject<APlayerCharacter>();

    for (int i = 0; i < 2; i++) {
        AABoardTile* Tile = NewObject<AABoardTile>();
        Tile->SpecialType = 2;
        Player->OwnedTiles.Add(Tile);
    }

    Player->UpdateRentBonus();
    TestEqual("2 special type2 tiles = 1.05f", Player->RentBonusType2, 1.05f);

    return true;
}

// Test 4: 3 fields, type 2
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBonusType2ThreeTilesTest, "Game.RentBonus.Type2.ThreeTiles", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FBonusType2ThreeTilesTest::RunTest(const FString& Parameters)
{
    APlayerCharacter* Player = NewObject<APlayerCharacter>();

    for (int i = 0; i < 3; i++)
    {
        AABoardTile* Tile = NewObject<AABoardTile>();
        Tile->SpecialType = 2;
        Player->OwnedTiles.Add(Tile);
    }

    Player->UpdateRentBonus();
    TestEqual("3 type2 tiles = 1.12f", Player->RentBonusType2, 1.12f);

    return true;
}

//Test 5: Type 2 for 4 fields
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBonusType2FourTilesTest, "Game.RentBonus.Type2.FourTiles", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FBonusType2FourTilesTest::RunTest(const FString& Parameters)
{
    APlayerCharacter* Player = NewObject<APlayerCharacter>();

    for (int i = 0; i < 4; i++) {
        AABoardTile* Tile = NewObject<AABoardTile>();
        Tile->SpecialType = 2;
        Player->OwnedTiles.Add(Tile);
    }

    Player->UpdateRentBonus();
    TestEqual("4 type2 tiles = 1.20f", Player->RentBonusType2, 1.20f);

    return true;
}

// Test 6: Type 2, 1 field
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBonusType2OneOrZeroTest, "Game.RentBonus.Type2.NotEnough", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FBonusType2OneOrZeroTest::RunTest(const FString& Parameters)
{
    APlayerCharacter* Player = NewObject<APlayerCharacter>();

    AABoardTile* Tile = NewObject<AABoardTile>();
    Tile->SpecialType = 2;
    Player->OwnedTiles.Add(Tile);

    Player->UpdateRentBonus();
    TestEqual("Less than 2 type2 gives no bonus", Player->RentBonusType2, 1.0f);

    return true;
}