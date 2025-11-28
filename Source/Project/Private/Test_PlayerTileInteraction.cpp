#include "Test_PlayerTileInteraction.h"
#include "Misc/AutomationTest.h"
#include "PlayerCharacter.h"
#include "ABoardTile.h"

//  Test 1 Buy Tile 
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPlayerBuyTileTest, "Game.PlayerTile.Buy", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FPlayerBuyTileTest::RunTest(const FString& Parameters)
{
    APlayerCharacter* Player = NewObject<APlayerCharacter>();
    AABoardTile* Tile = NewObject<AABoardTile>();

    Player->Money = 1500;
    Player->PlayerID = 1;
    Tile->TileCost = 200;
    Tile->bCanBeBought = true;

    // Simulate purchase
    if (Player->Money >= Tile->TileCost && Tile->OwnerID == -1) {
        Player->RemoveMoney(Tile->TileCost);
        Tile->OwnerID = Player->PlayerID;
        Player->AddOwnedTile(Tile);
    }

    TestEqual("Money reduced by purchase cost", Player->Money, 1300);
    TestEqual("Tile should belong to player 1", Tile->OwnerID, 1);
    TestEqual("Player should have 1 owned tile", Player->OwnedTiles.Num(), 1);

    return true;
}


//  Test 2 Rent Paid to Another Player 
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPlayerPayRentTest, "Game.PlayerTile.Rent.Payment", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FPlayerPayRentTest::RunTest(const FString& Parameters)
{
    APlayerCharacter* Payer = NewObject<APlayerCharacter>();
    APlayerCharacter* Owner = NewObject<APlayerCharacter>();
    AABoardTile* Tile = NewObject<AABoardTile>();

    Payer->Money = 1000;
    Owner->Money = 500;
    Owner->PlayerID = 5;

    Tile->RentRate = 0.2f;
    Tile->TileCost = 1000;
    Tile->OwnerID = 5;

    int32 Rent = Tile->GetBaseRent();

    Payer->RemoveMoney(Rent);
    Owner->AddMoney(Rent);

    TestEqual("Payer loses rent", Payer->Money, 1000 - Rent);
    TestEqual("Owner receives rent", Owner->Money, 500 + Rent);

    return true;
}


//  Test 3 No Rent If Tile Belongs to Player 
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPlayerNoRentOnOwnTileTest, "Game.PlayerTile.Rent.NoSelfRent", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FPlayerNoRentOnOwnTileTest::RunTest(const FString& Parameters)
{
    APlayerCharacter* Player = NewObject<APlayerCharacter>();
    AABoardTile* Tile = NewObject<AABoardTile>();

    Player->Money = 1500;
    Player->PlayerID = 3;

    Tile->TileCost = 200;
    Tile->OwnerID = 3;

    int32 RentBefore = Player->Money;

    if (Tile->OwnerID != Player->PlayerID) {
        Player->RemoveMoney(Tile->GetBaseRent());
    }

    TestEqual("Money stays the same on own tile", Player->Money, RentBefore);

    return true;
}