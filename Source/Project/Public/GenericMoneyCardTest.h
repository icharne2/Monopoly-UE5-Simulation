#pragma once

#include "CoreMinimal.h"
#include "FunctionalTest.h"
#include "GenericMoneyCardTest.generated.h"

UENUM()
enum class EGenericCardPhase : uint8
{
    Phase_Idle,
    Phase_EffectApplied,
    Phase_Finished
};

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
    UPROPERTY(EditAnywhere, Category = "Test Settings")
    int32 CardIDToTest;

    UPROPERTY(EditAnywhere, Category = "Test Settings")
    int32 ExpectedChange;

    UPROPERTY(EditAnywhere, Category = "Test Settings")
    int32 TargetPlayerID;

private:
    EGenericCardPhase CurrentPhase;
    int32 InitialMoney;
};