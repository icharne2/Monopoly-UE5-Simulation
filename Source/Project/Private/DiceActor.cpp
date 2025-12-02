#include "DiceActor.h"

ADiceActor::ADiceActor()
{
    // Enable Tick to constantly check the dice's movement
	PrimaryActorTick.bCanEverTick = true;

    // Create the main 3D model for the dice and enable physics so it can roll
    DiceMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DiceMesh"));
    RootComponent = DiceMesh;
    DiceMesh->SetSimulatePhysics(true);

    // Create 6 arrows, each one indicates the normal direction of a given face
    Arrow1 = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow1"));
    Arrow1->SetupAttachment(DiceMesh);

    Arrow2 = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow2"));
    Arrow2->SetupAttachment(DiceMesh);

    Arrow3 = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow3"));
    Arrow3->SetupAttachment(DiceMesh);

    Arrow4 = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow4"));
    Arrow4->SetupAttachment(DiceMesh);

    Arrow5 = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow5"));
    Arrow5->SetupAttachment(DiceMesh);

    Arrow6 = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow6"));
    Arrow6->SetupAttachment(DiceMesh);

    // Detection variables
    PrevLocation = FVector::ZeroVector;
    PrevRotation = FQuat::Identity;
    StableFrames = 0;

}

void ADiceActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Main logic loop: Checks every frame if the dice has stopped rolling
void ADiceActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bIsRolling)
        return;

    // Get current movement speed (linear) and rotation speed (angular)
    const FVector LinVel = DiceMesh->GetPhysicsLinearVelocity();
    const FVector AngVel = DiceMesh->GetPhysicsAngularVelocityInDegrees();

    // Calculate how much position and rotation changed since the last frame
    const float PosDelta = FVector::Dist(PrevLocation, GetActorLocation());
    const float RotDelta = FQuat::ErrorAutoNormalize(PrevRotation, GetActorQuat());

    // Update previous location for the next check
    PrevLocation = GetActorLocation();
    PrevRotation = GetActorQuat();

    // Check conditions. Is the dice moving slowly enough to be considered "stopped" ?
    const bool bLinStill = LinVel.Size() < LinearStopThreshold;
    const bool bAngStill = AngVel.Size() < AngularStopThreshold;
    const bool bPosStill = PosDelta < 0.5f;     // does not slide across the floor
    const bool bRotStill = RotDelta < 0.0015f;  // it doesn't swing

    if (bLinStill && bAngStill && bPosStill && bRotStill) {
        // another stable frame
        StableFrames++;

        // when we collect a few calm frame, we consider that it stands
        if (StableFrames >= RequiredStableFrames) {
            bIsRolling = false;
            StableFrames = 0;

            CheckDiceResult();

            GetWorld()->GetTimerManager().SetTimerForNextTick([this]() { OnDiceStopped.Broadcast(); });

            UE_LOG(LogTemp, Warning, TEXT("Dice stopped, top face = %d"), DiceResult);
        }
    } else {
        // if it moves even a little, we start counting again
        StableFrames = 0;
    }
}

void ADiceActor::RollDice()
{
    bIsRolling = true;
    TimeStill = 0.f;

    // Stop current movement before applying new force
    DiceMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
    DiceMesh->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);

    // Random force to throw the dice (Upwards + Random X/Y direction)
    FVector Impulse = FVector(
        FMath::RandRange(-60, 60),
        FMath::RandRange(-60, 60),
        FMath::RandRange(400, 600)
    );

    // Random rotation force (Spin)
    FVector Torque = FVector(
        FMath::RandRange(-720, 720),
        FMath::RandRange(-720, 720),
        FMath::RandRange(-720, 720)
    );

    // Apply the forces
    DiceMesh->AddImpulse(Impulse, NAME_None, true);
    DiceMesh->AddAngularImpulseInDegrees(Torque, NAME_None, true);
}

// Determines which number is on top
void ADiceActor::CheckDiceResult()
{
    TArray<UArrowComponent*> Arrows = { Arrow1, Arrow2, Arrow3, Arrow4, Arrow5, Arrow6 };

    FVector UpVector = FVector::UpVector; // Global Up direction (0, 0, 1)
    float MaxDot = -1.f;
    int32 TopFaceIndex = 0;

    for (int32 i = 0; i < Arrows.Num(); i++) {
        if (!Arrows[i]) continue;

        const float Dot = FVector::DotProduct(Arrows[i]->GetForwardVector(), UpVector);

        // Find the arrow with the highest alignment to "Up"
        if (Dot > MaxDot) {
            MaxDot = Dot;
            TopFaceIndex = i + 1;
        }
    }

    DiceResult = TopFaceIndex;
}

