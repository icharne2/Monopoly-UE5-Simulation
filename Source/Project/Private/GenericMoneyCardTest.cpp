#include "GenericMoneyCardTest.h"
#include "MyGameMode.h"
#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "HubMainWidget.h"
#include "TimerManager.h"

AGenericMoneyCardTest::AGenericMoneyCardTest()
{
    TimeLimit = 5.0f;
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;

    // Default values
    CardIDToTest = 1;
    ExpectedChange = 200;
    TargetPlayerID = 1;
}

void AGenericMoneyCardTest::BeginPlay()
{
    Super::BeginPlay();

    AMyGameMode* GM = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
    if (GM && GM->CachedHubWidget) GM->CachedHubWidget->SetVisibility(ESlateVisibility::Hidden);

    FTimerHandle Handle;
    GetWorld()->GetTimerManager().SetTimer(Handle, this, &AGenericMoneyCardTest::StartTest, 1.0f, false);
}

void AGenericMoneyCardTest::StartTest()
{
    AMyGameMode* GM = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
    if (!GM) { FinishTest(EFunctionalTestResult::Failed, TEXT("Brak GameMode")); return; }

    if (GM->PlayerCharacters.Num() == 0)
        GM->StartGameSetup();


    APlayerCharacter* Player = GM->GetPlayerByID(TargetPlayerID);

    if (!Player){
        FString Err = FString::Printf(TEXT("Nie znaleziono gracza o ID: %d"), TargetPlayerID);
        FinishTest(EFunctionalTestResult::Failed, Err);
        return;
    }

    InitialMoney = Player->Money;

    GM->PendingChancePlayerID = TargetPlayerID; 
    GM->PendingChanceEffect = CardIDToTest;

    GM->ApplyPendingChanceEffect();

    CurrentPhase = EGenericCardPhase::Phase_EffectApplied;
}

void AGenericMoneyCardTest::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (CurrentPhase != EGenericCardPhase::Phase_EffectApplied) return;

    AMyGameMode* GM = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

    APlayerCharacter* Player = GM->GetPlayerByID(TargetPlayerID);
    if (!Player) return;

    int32 FinalMoney = Player->Money;
    int32 ActualChange = FinalMoney - InitialMoney;

    if (ActualChange == ExpectedChange) {
        FinishTest(EFunctionalTestResult::Succeeded, TEXT("SUKCES: Stan konta zmienil sie poprawnie."));
    } else {
        FString Err = FString::Printf(TEXT("BLAD: Oczekiwano zmiany o %d, a zmienilo sie o %d. (Start: %d, Koniec: %d)"),
            ExpectedChange, ActualChange, InitialMoney, FinalMoney);
        FinishTest(EFunctionalTestResult::Failed, Err);
    }
    CurrentPhase = EGenericCardPhase::Phase_Finished;
}
