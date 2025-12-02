#pragma once

#include "CoreMinimal.h"
#include "FunctionalTest.h"
#include "DiceHudTest.generated.h"

UENUM()
enum class EDiceTestPhase : uint8
{
    Phase_Idle,         // Waiting for the game to initializ
    Phase_Rolling,      // The dice have been thrown
    Phase_Finished      // The test has completed (Success / Fail).
};

/**
 * ADiceHudTest
 * A Functional Test actor that automates the dice rolling process.
 * - Force the GameMode to roll the dice.
 * - Set the camera so we can see it.
 * - Wait for the HUD (UI) to display the result text.
 */

UCLASS()
class PROJECT_API ADiceHudTest : public AFunctionalTest
{
    GENERATED_BODY()

public:
    ADiceHudTest();

    virtual void BeginPlay() override;
    virtual void StartTest() override;
    virtual void Tick(float DeltaSeconds) override;

private:
    // Keeps track of which step the test is currently in.
    EDiceTestPhase CurrentPhase;
};