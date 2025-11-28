#include "DiceHudTest.h"
#include "MyGameMode.h"
#include "HubMainWidget.h"
#include "DiceActor.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include "TimerManager.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraActor.h" 

ADiceHudTest::ADiceHudTest()
{
    TimeLimit = 30.0f;
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;
    CurrentPhase = EDiceTestPhase::Phase_Idle;
}

void ADiceHudTest::BeginPlay()
{
    Super::BeginPlay();

    FTimerHandle UnusedHandle;
    GetWorld()->GetTimerManager().SetTimer(UnusedHandle, this, &ADiceHudTest::StartTest, 0.1f, false);
}

void ADiceHudTest::StartTest()
{
    AMyGameMode* GM = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
    if (!GM) {
        FinishTest(EFunctionalTestResult::Failed, TEXT("Brak GameMode!"));
        return;
    }

    if (!GM->Dice1Ref || !GM->Dice2Ref) {
        TArray<AActor*> FoundDice;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADiceActor::StaticClass(), FoundDice);

        if (FoundDice.Num() >= 2) {
            GM->Dice1Ref = Cast<ADiceActor>(FoundDice[0]);
            GM->Dice2Ref = Cast<ADiceActor>(FoundDice[1]);
        } else {
            FinishTest(EFunctionalTestResult::Failed, TEXT("Wymagane są dwie kostki!"));
            return;
        }
    }

    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);

    TArray<AActor*> FoundCameras;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACameraActor::StaticClass(), FoundCameras);

    if (PC && FoundCameras.Num() > 0) {
        AActor* MySceneCamera = FoundCameras[0];
        PC->SetViewTarget(MySceneCamera);
    } else {
        LogStep(ELogVerbosity::Error, TEXT("Błąd: Nie znaleziono CameraActor na mapie!"));
    }

    GM->RollDice();

    CurrentPhase = EDiceTestPhase::Phase_Rolling;
}

void ADiceHudTest::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (CurrentPhase != EDiceTestPhase::Phase_Rolling) return;

    AMyGameMode* GM = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
    if (!GM || !GM->CachedHubWidget) return;

}