#include "Test_BoardTileLogic.h"
#include "Misc/AutomationTest.h"
#include "ABoardTile.h"

//  Test 1 Base Rent 
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBoardTile_BaseRentTest, "Game.Tile.Rent.Base", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FBoardTile_BaseRentTest::RunTest(const FString& Parameters)
{
    AABoardTile* Tile = NewObject<AABoardTile>();

    Tile->TileCost = 1000;
    Tile->RentRate = 0.2f;

    TestEqual("Base rent should be 200", Tile->GetBaseRent(), 200);

    return true;
}


//  Test 2 Upgraded Rent 
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBoardTile_UpgradedRentTest, "Game.Tile.Rent.Upgraded", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FBoardTile_UpgradedRentTest::RunTest(const FString& Parameters)
{
    AABoardTile* Tile = NewObject<AABoardTile>();

    Tile->TileCost = 1000;
    Tile->RentRate = 0.2f;
    Tile->UpgradeIncomeBonuses = { 0.0f, 0.1f, 0.2f, 0.4f };

    Tile->UpgradeLevel = 0;
    TestEqual("Rent L0 = 200", Tile->GetUpgradedRent(), 200);

    Tile->UpgradeLevel = 1;
    TestEqual("Rent L1 = 220", Tile->GetUpgradedRent(), 220);

    Tile->UpgradeLevel = 2;
    TestEqual("Rent L2 = 240", Tile->GetUpgradedRent(), 240);

    Tile->UpgradeLevel = 3;
    TestEqual("Rent L3 = 280", Tile->GetUpgradedRent(), 280);

    return true;
}

//  TEST 3: Mortgage Values 
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBoardTile_MortgageTest, "Game.Tile.Mortgage", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FBoardTile_MortgageTest::RunTest(const FString& Parameters)
{
    AABoardTile* Tile = NewObject<AABoardTile>();
    Tile->TileCost = 1200;
    Tile->MortgageRate = 0.5f;
    Tile->MortgageInterestRate = 0.1f;

    TestEqual("Mortgage value is 600", Tile->GetMortgageValue(), 600);
    TestEqual("Redeem cost should be 660", Tile->GetRedeemCost(), 660);

    return true;
}

//  Test 4 Upgrade Restrictions 
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBoardTile_MaxUpgradeTest, "Game.Tile.Upgrade.Restrictions", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FBoardTile_MaxUpgradeTest::RunTest(const FString& Parameters)
{
    AABoardTile* Tile = NewObject<AABoardTile>();

    Tile->UpgradeIncomeBonuses = { 0.0f, 0.1f, 0.2f, 0.4f }; // max level index = 3
    Tile->UpgradeLevel = 3;

    // Attempt to upgrade when already maxed
    bool bResult = Tile->UpgradeTile();

    TestFalse("Cannot upgrade beyond max level", bResult);
    TestEqual("Level should remain 3", Tile->UpgradeLevel, 3);

    return true;
}