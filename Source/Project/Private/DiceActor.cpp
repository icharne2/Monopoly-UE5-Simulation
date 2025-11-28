#include "DiceActor.h"

ADiceActor::ADiceActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    DiceMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DiceMesh"));
    RootComponent = DiceMesh;
    DiceMesh->SetSimulatePhysics(true);

    // We create 6 arrows - each one indicates the normal direction of a given face
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

    PrevLocation = FVector::ZeroVector;
    PrevRotation = FQuat::Identity;
    StableFrames = 0;

}

void ADiceActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADiceActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bIsRolling)
        return;

    // Current speeds
    const FVector LinVel = DiceMesh->GetPhysicsLinearVelocity();
    const FVector AngVel = DiceMesh->GetPhysicsAngularVelocityInDegrees();

    // Change position/rotation between frames
    const float PosDelta = FVector::Dist(PrevLocation, GetActorLocation());
    const float RotDelta = FQuat::ErrorAutoNormalize(PrevRotation, GetActorQuat());

    // Remember for the next frame
    PrevLocation = GetActorLocation();
    PrevRotation = GetActorQuat();

    // Ss the cube "almost motionless" in this frame
    const bool bLinStill = LinVel.Size() < LinearStopThreshold;
    const bool bAngStill = AngVel.Size() < AngularStopThreshold;
    const bool bPosStill = PosDelta < 0.5f;     // does not slide across the floor
    const bool bRotStill = RotDelta < 0.0015f;  // it doesn't swing

    if (bLinStill && bAngStill && bPosStill && bRotStill) {
        // another stable frame
        StableFrames++;

        // when we collect a few calm frames - we consider that it stands
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

    DiceMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
    DiceMesh->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);

    FVector Impulse = FVector(
        FMath::RandRange(-60, 60),
        FMath::RandRange(-60, 60),
        FMath::RandRange(400, 600)
    );

    FVector Torque = FVector(
        FMath::RandRange(-720, 720),
        FMath::RandRange(-720, 720),
        FMath::RandRange(-720, 720)
    );

    DiceMesh->AddImpulse(Impulse, NAME_None, true);
    DiceMesh->AddAngularImpulseInDegrees(Torque, NAME_None, true);
}

void ADiceActor::CheckDiceResult()
{
    TArray<UArrowComponent*> Arrows = { Arrow1, Arrow2, Arrow3, Arrow4, Arrow5, Arrow6 };

    FVector UpVector = FVector::UpVector;
    float MaxDot = -1.f;
    int32 TopFaceIndex = 0;

    for (int32 i = 0; i < Arrows.Num(); i++) {
        if (!Arrows[i]) continue;

        const float Dot = FVector::DotProduct(Arrows[i]->GetForwardVector(), UpVector);

        if (Dot > MaxDot) {
            MaxDot = Dot;
            TopFaceIndex = i + 1;
        }
    }

    DiceResult = TopFaceIndex;
}

