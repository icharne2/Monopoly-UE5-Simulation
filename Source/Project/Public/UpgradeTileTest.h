#pragma once

#include "CoreMinimal.h"
#include "FunctionalTest.h"
#include "UpgradeTileTest.generated.h"

UENUM()
enum class EUpgradePhase : uint8
{
    Phase_Idle,             // waiting 1s
    Phase_Verification,     // waiting for mesh
    Phase_Finished
};

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