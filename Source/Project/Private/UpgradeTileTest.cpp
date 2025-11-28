#include "UpgradeTileTest.h"
#include "ABoardTile.h"
#include "MyGameMode.h"
#include "HubMainWidget.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraActor.h"
#include "GameFramework/PlayerController.h"

AUpgradeTileTest::AUpgradeTileTest()
{
    TimeLimit = 10.0f;
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;
    CurrentPhase = EUpgradePhase::Phase_Idle;
}

void AUpgradeTileTest::BeginPlay()
{
    Super::BeginPlay();

    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    TArray<AActor*> FoundCameras;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACameraActor::StaticClass(), FoundCameras);

    if (PC && FoundCameras.Num() > 0)
        PC->SetViewTarget(FoundCameras[0]);

    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AUpgradeTileTest::PerformUpgrade, 1.5f, false);
}

void AUpgradeTileTest::PerformUpgrade()
{
    AActor* FoundActor = UGameplayStatics::GetActorOfClass(GetWorld(), AABoardTile::StaticClass());
    TargetTile = Cast<AABoardTile>(FoundActor);

    if (!TargetTile) {
        FinishTest(EFunctionalTestResult::Failed, TEXT("Brak pola (BP_BoardTile) na mapie!"));
        return;
    }

    TargetTile->OwnerID = 1;
    TargetTile->bCanBeUpdate = true;
    TargetTile->UpgradeLevel = 0;

    bool bResult = TargetTile->UpgradeTile();

    if (bResult){
        CurrentPhase = EUpgradePhase::Phase_Verification;
    } else {
        FinishTest(EFunctionalTestResult::Failed, TEXT("UpgradeTile() zwrocilo false!"));
    }
}

void AUpgradeTileTest::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (CurrentPhase != EUpgradePhase::Phase_Verification) return;
    if (!TargetTile) return;

    UStaticMeshComponent* UpgradeMeshComp = nullptr;

    TArray<UActorComponent*> Components;
    TargetTile->GetComponents(UStaticMeshComponent::StaticClass(), Components);

    for (UActorComponent* Comp : Components){
        if (Comp->GetName() == TEXT("UpgradeMesh")){
            UpgradeMeshComp = Cast<UStaticMeshComponent>(Comp);
            break;
        }
    }

    if (UpgradeMeshComp){
        if (UpgradeMeshComp->IsVisible()){
            FinishTest(EFunctionalTestResult::Succeeded, TEXT("SUKCES: Mesh pojawil sie na polu!"));
            CurrentPhase = EUpgradePhase::Phase_Finished;
        }
    }
}