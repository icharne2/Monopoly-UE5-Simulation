#pragma once

#include "CoreMinimal.h"
#include "FunctionalTest.h"
#include "GenericMoneyCardTest.generated.h"

UENUM()
enum class EGenericCardPhase : uint8
{
    Phase_Idle,             // Waiting for the game to start
    Phase_EffectApplied,    // The card effect has been triggered, waiting to check the result
    Phase_Finished          // The test has completed (Success / Fail).
};

/**
 * AGenericMoneyCardTest
 * A flexible test for checking "Money" Chance Cards.
 * * Instead of writing a separate C++ file for every card,
 * you place this Blueprint on the map and configure the numbers in the "Details" panel.
 */

UCLASS()
class PROJECT_API AGenericMoneyCardTest : public AFunctionalTest
{
    GENERATED_BODY()

public:
    AGenericMoneyCardTest();

    virtual void BeginPlay() override;
    virtual void StartTest() override;
    virtual void Tick(float DeltaSeconds) override;

protected:
    // The ID of the Chance Card to test
    UPROPERTY(EditAnywhere, Category = "Test Settings")
    int32 CardIDToTest;

    // How much money the player is expected to gain or lose.
    UPROPERTY(EditAnywhere, Category = "Test Settings")
    int32 ExpectedChange;

    // The ID of the player to test this on
    UPROPERTY(EditAnywhere, Category = "Test Settings")
    int32 TargetPlayerID;

private:
    // Internal state to track progress
    EGenericCardPhase CurrentPhase;

    // Stores how much money the player had BEFORE the card effect
    int32 InitialMoney;
};