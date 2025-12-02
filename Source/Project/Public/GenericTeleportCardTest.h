#pragma once

#include "CoreMinimal.h"
#include "FunctionalTest.h"
#include "GenericTeleportCardTest.generated.h"

UENUM()
enum class ETeleportTestPhase : uint8
{
    Phase_Idle,             // Waiting for the game to start
    Phase_EffectApplied,    // The card effect has been triggered, waiting to check the result
    Phase_Finished          // The test has completed (Success / Fail).
};

/**
 * AGenericTeleportCardTest
 * A flexible test for checking Chance Cards that move the player.
 * * Instead of writing separate code for "Go to Jail" and "Go to Start",
 * you use this single class and just change the IDs in the Editor.
 */

UCLASS()
class PROJECT_API AGenericTeleportCardTest : public AFunctionalTest
{
    GENERATED_BODY()

public:
    AGenericTeleportCardTest();

    virtual void BeginPlay() override;
    virtual void StartTest() override;
    virtual void Tick(float DeltaSeconds) override;

protected:

    // The ID of the Chance Card to test.
    // 12 = Jail, 5 = Start, 14 = Library
    UPROPERTY(EditAnywhere, Category = "Test Settings")
    int32 CardIDToTest;

    // The Tile ID where the player is expected to land.
    UPROPERTY(EditAnywhere, Category = "Test Settings")
    int32 ExpectedTileID;

    // The ID of the player to test this on
    UPROPERTY(EditAnywhere, Category = "Test Settings")
    int32 TargetPlayerID;

    // Delay before this test starts
    UPROPERTY(EditAnywhere, Category = "Test Settings")
    float TestStartDelay;

private:
    // Internal state to track progress
    ETeleportTestPhase CurrentPhase;

    // Stores where the player was before the teleport
    int32 InitialTileIndex;
};
