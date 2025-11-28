#include "Test_TileMortgage.h"
#include "Misc/AutomationTest.h"
#include "PlayerCharacter.h"
#include "ABoardTile.h"
#include "MyGameMode.h"


// Test 1 Mortgage 
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTileMortgageTest, "Game.Tile.Mortgage.Basic", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FTileMortgageTest::RunTest(const FString& Parameters)
{
    APlayerCharacter* Player = NewObject<APlayerCharacter>();
    AABoardTile* Tile = NewObject<AABoardTile>();

    Player->Money = 1000;
    Tile->TileCost = 500;
    Tile->MortgageRate = 0.5f;  
    Tile->OwnerID = 1;
    Player->PlayerID = 1;

    int32 MortgageValue = Tile->GetMortgageValue(); 
    Player->AddMoney(MortgageValue);
    Tile->bIsMortgaged = true;

    TestEqual("Player receives mortgage value", Player->Money, 1000 + MortgageValue);
    TestTrue("Tile should be marked as mortgaged", Tile->bIsMortgaged);

    return true;
}

//  Test 2: Redeem  
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTileRedeemTest, "Game.Tile.Mortgage.Redeem", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FTileRedeemTest::RunTest(const FString& Parameters)
{
    APlayerCharacter* Player = NewObject<APlayerCharacter>();
    AABoardTile* Tile = NewObject<AABoardTile>();

    Player->Money = 1000;
    Tile->TileCost = 500;
    Tile->MortgageRate = 0.5f;
    Tile->MortgageInterestRate = 0.1f;
    Tile->bIsMortgaged = true;

    int32 RedeemCost = Tile->GetRedeemCost(); 

    Player->RemoveMoney(RedeemCost);
    Tile->bIsMortgaged = false;

    TestEqual("Money reduced by redeem cost", Player->Money, 1000 - RedeemCost);
    TestFalse("Tile no longer mortgaged", Tile->bIsMortgaged);

    return true;
}


//  Test 3 Cannot Redeem Without Money 
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTileRedeemFailTest, "Game.Tile.Mortgage.NoMoney", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FTileRedeemFailTest::RunTest(const FString& Parameters)
{
    APlayerCharacter* Player = NewObject<APlayerCharacter>();
    AABoardTile* Tile = NewObject<AABoardTile>();

    Player->Money = 50;          
    Tile->TileCost = 500;
    Tile->bIsMortgaged = true;

    int32 RedeemCost = Tile->GetRedeemCost();

    if (Player->Money >= RedeemCost) {
        Player->RemoveMoney(RedeemCost);
        Tile->bIsMortgaged = false;
    }

    TestEqual("Tile stays mortgaged without enough money", Tile->bIsMortgaged, true);

    return true;
}


// Test 4
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBankruptcyOwnershipCleanupTest, "Game.Bankruptcy.Ownership.Reset", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FBankruptcyOwnershipCleanupTest::RunTest(const FString& Parameters)
{

    APlayerCharacter* Player = NewObject<APlayerCharacter>();
    Player->PlayerID = 1;
 
    TArray<AABoardTile*> TilesBefore;
    for (int i = 0; i < 3; i++) {
        AABoardTile* Tile = NewObject<AABoardTile>();
        Tile->TileID = i + 1;
        Tile->OwnerID = 1;
        Tile->UpgradeLevel = 2;
        Tile->bIsMortgaged = true;
        Player->OwnedTiles.Add(Tile);

        TilesBefore.Add(Tile); 
    }

    Player->DeclareBankruptcy();

    for (AABoardTile* Tile : TilesBefore) {
        TestEqual("OwnerID reset to -1 after bankruptcy", Tile->OwnerID, -1);
        TestEqual("Upgrades removed after bankruptcy", Tile->UpgradeLevel, 0);
        TestFalse("Tile is no longer mortgaged after bankruptcy", Tile->bIsMortgaged);
    }

    TestEqual("Player's OwnedTiles list cleared after bankruptcy", Player->OwnedTiles.Num(), 0);

    return true;
}


