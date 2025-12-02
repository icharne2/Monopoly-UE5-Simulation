#include "GenericTeleportCardTest.h"
#include "MyGameMode.h"
#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "HubMainWidget.h"
#include "TimerManager.h"
#include "GameFramework/PlayerController.h" 

AGenericTeleportCardTest::AGenericTeleportCardTest()
{
    TimeLimit = 10.0f;
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;

    CardIDToTest = 12;
    ExpectedTileID = 10;
    TargetPlayerID = 1;
    TestStartDelay = 1.0f;
    CurrentPhase = ETeleportTestPhase::Phase_Idle;
}

void AGenericTeleportCardTest::BeginPlay()
{
    Super::BeginPlay();

    FTimerHandle Handle;
    GetWorld()->GetTimerManager().SetTimer(Handle, this, &AGenericTeleportCardTest::StartTest, TestStartDelay, false);
}

void AGenericTeleportCardTest::StartTest()
{
    AMyGameMode* GM = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
    if (!GM) { FinishTest(EFunctionalTestResult::Failed, TEXT("Brak GameMode")); return; }

    if (GM->PlayerCharacters.Num() == 0) GM->StartGameSetup();

    APlayerCharacter* Player = GM->GetPlayerByID(TargetPlayerID);
    if (!Player){
        FinishTest(EFunctionalTestResult::Failed, TEXT("Brak Gracza"));
        return;
    }

    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PC) PC->SetViewTarget(Player);

    GM->PendingChancePlayerID = TargetPlayerID;
    GM->PendingChanceEffect = CardIDToTest;

    GM->ApplyPendingChanceEffect();

    CurrentPhase = ETeleportTestPhase::Phase_EffectApplied;
}

void AGenericTeleportCardTest::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (CurrentPhase != ETeleportTestPhase::Phase_EffectApplied) return;

    AMyGameMode* GM = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
    APlayerCharacter* Player = GM->GetPlayerByID(TargetPlayerID);
    if (!Player) return;


    if (Player->CurrentTileIndex == ExpectedTileID){
        FString Msg = FString::Printf(TEXT("SUKCES: Gracz przeniesiony na pole %d."), ExpectedTileID);
        FinishTest(EFunctionalTestResult::Succeeded, Msg);
    } else {
        FString Err = FString::Printf(TEXT("BLAD: Gracz jest na polu %d, a mial byc na %d."), Player->CurrentTileIndex, ExpectedTileID);
        FinishTest(EFunctionalTestResult::Failed, Err);
    }

    CurrentPhase = ETeleportTestPhase::Phase_Finished;
}
