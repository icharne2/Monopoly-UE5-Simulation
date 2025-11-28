#include "Test_PlayerMoney.h"          
#include "Misc/AutomationTest.h"
#include "PlayerCharacter.h"

// Test 1: AddMoney basic behavior
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPlayerMoneyAddTest, "Game.Player.Money.Add", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FPlayerMoneyAddTest::RunTest(const FString& Parameters)
{
    APlayerCharacter* Player = NewObject<APlayerCharacter>(GetTransientPackage(), APlayerCharacter::StaticClass());
    TestNotNull(TEXT("Player should be created"), Player);

    Player->Money = 1000;
    Player->AddMoney(500);

    TestEqual(TEXT("Money should be 1500 after adding 500"), Player->Money, 1500);

    Player->AddMoney(0);
    TestEqual(TEXT("Money should remain 1500 after adding 0"), Player->Money, 1500);

    return true;
}

// Test 2: RemoveMoney basic behavior
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPlayerMoneyRemoveTest, "Game.Player.Money.Remove", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FPlayerMoneyRemoveTest::RunTest(const FString& Parameters)
{
    APlayerCharacter* Player = NewObject<APlayerCharacter>(GetTransientPackage(), APlayerCharacter::StaticClass());
    TestNotNull(TEXT("Player should be created"), Player);

    Player->Money = 1000;
    Player->RemoveMoney(300);
    TestEqual(TEXT("Money should be 700 after removing 300"), Player->Money, 700);

    // Edge Case: Remove zero
    Player->RemoveMoney(0);
    TestEqual(TEXT("Money should stay 700 after removing 0"), Player->Money, 700);

    return true;
}

// Test 3: Money should not go below zero (if your game logic enforces this)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPlayerMoneyNoNegativeTest, "Game.Player.Money.NoNegative", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FPlayerMoneyNoNegativeTest::RunTest(const FString& Parameters)
{
    APlayerCharacter* Player = NewObject<APlayerCharacter>(GetTransientPackage(), APlayerCharacter::StaticClass());
    TestNotNull(TEXT("Player should be created"), Player);

    Player->Money = 100;
    Player->RemoveMoney(500);

    TestTrue(TEXT("Money should not be negative"), Player->Money >= 0);

    // Check bankruptcy flag if exists
    if (Player->GetClass()->FindPropertyByName(TEXT("bIsBankrupt"))){
        TestTrue(TEXT("Player should be bankrupt (bIsBankrupt == true)"), Player->bIsBankrupt);
    }

    return true;
}
