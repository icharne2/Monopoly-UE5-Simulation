#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/ArrowComponent.h"
#include "DiceActor.generated.h"

// Event delegate that allows other classes (like GameMode) to listen for when the dice stops.
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDiceStopped);

/**
 * ADiceActor
 * Represents a physical dice in the game.
 * It simulates physics (rolling), detects when it has stopped moving,
 * and calculates the result based on which face is pointing up.
 */

UCLASS()
class PROJECT_API ADiceActor : public AActor
{
	GENERATED_BODY()

public:	
    // Sets default values for this actor's properties
    ADiceActor();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

    // Rolls the dice
    UFUNCTION(BlueprintCallable, Category = "Dice")
    void RollDice();

    // Returns the final rolled value
    UFUNCTION(BlueprintCallable, Category = "Dice")
    int32 GetDiceResult() const { return DiceResult; }

    // Event fired when dice has stopped
    UPROPERTY(BlueprintAssignable, Category = "Dice")
    FOnDiceStopped OnDiceStopped;

    // DETECTION SETTINGS

    // How many frames must be "calm" to be considered as standing
    UPROPERTY(EditAnywhere, Category = "Dice|Detect")
    int32 RequiredStableFrames = 5;  //5 frames at 60 fps ≈ 0.08 s

    // Max allowed linear movement to count as stable
    UPROPERTY(EditAnywhere, Category = "Dice|Detect")
    float LinearStopThreshold = 4.0f;

    // Max allowed angular velocity to count as stable
    UPROPERTY(EditAnywhere, Category = "Dice|Detect")
    float AngularStopThreshold = 4.0f;

    //Dice mesh
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dice")
    UStaticMeshComponent* DiceMesh;

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    // Checks if dice is stable and detects final face
    void CheckDiceResult();

    // INTERNAL STATE

    // Whether the dice is currently rolling
    bool bIsRolling = false;

    // Latest obtained value
    int32 DiceResult = 0;

    // Used to detect stillness
    float TimeStill = 0.f;

    // Seconds threshold to consider dice stopped
    float StoppedThreshold = 5.f;

    // For precise stop detection
    // Previous frame location
    FVector PrevLocation;

    // Previous frame rotation
    FQuat   PrevRotation;

    // How many consecutive stable frames we have
    int32   StableFrames = 0;

    // COMPONENTS
    // Dice arrows
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dice")
    UArrowComponent* Arrow1;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dice")
    UArrowComponent* Arrow2;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dice")
    UArrowComponent* Arrow3;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dice")
    UArrowComponent* Arrow4;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dice")
    UArrowComponent* Arrow5;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dice")
    UArrowComponent* Arrow6;

};
