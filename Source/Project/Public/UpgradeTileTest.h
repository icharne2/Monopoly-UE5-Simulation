#pragma once

#include "CoreMinimal.h"
#include "FunctionalTest.h"
#include "UpgradeTileTest.generated.h"

UENUM()
enum class EUpgradePhase : uint8
{
    Phase_Idle,             // Waiting for the game to start
    Phase_Verification,     // Waiting for mesh
    Phase_Finished          // The test has completed (Success / Fail).
};

/**
 * AUpgradeTileTest
 * A Functional Test that checks the property upgrade system.
 * * It finds a tile, forces an upgrade (buying a house), and then checks if the
 * 3D model (mesh) actually became visible on the screen.
 */

UCLASS()
class PROJECT_API AUpgradeTileTest : public AFunctionalTest
{
    GENERATED_BODY()

public:
    AUpgradeTileTest();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

    void PerformUpgrade();

private:
    EUpgradePhase CurrentPhase;

    // Test tile
    class AABoardTile* TargetTile;
};
