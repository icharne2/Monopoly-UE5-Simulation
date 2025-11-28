#pragma once

#include "CoreMinimal.h"
#include "FunctionalTest.h"
#include "DiceHudTest.generated.h"

UENUM()
enum class EDiceTestPhase : uint8
{
    Phase_Idle,
    Phase_Rolling,
    Phase_Finished
};

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
    EDiceTestPhase CurrentPhase;
};