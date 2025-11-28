#include "MyGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerCharacter.h"
#include "ABoardTile.h"
#include "MyGameInstance.h"
#include "HubMainWidget.h"
#include "DiceActor.h"
#include "Components/TextBlock.h"
#include "Camera/CameraActor.h"
#include "Camera/PlayerCameraManager.h"
#include "GameFramework/PlayerController.h"


AMyGameMode::AMyGameMode()
{
    PrimaryActorTick.bCanEverTick = false;
}

APlayerCharacter* AMyGameMode::GetPlayerByID(int32 PlayerID)
{
    for (APlayerCharacter* PC : PlayerCharacters) {
        if (PC && PC->PlayerID == PlayerID)
            return PC;
    }
    return nullptr;
}

void AMyGameMode::BeginPlay()
{
    Super::BeginPlay();

    // CHANCE CARDS
    ChanceCards.Add(FChanceCard(TEXT("Zaliczenie bez poprawki! Otrzymujesz 200 ZR od Uczelni."), 1));
    ChanceCards.Add(FChanceCard(TEXT("Projekt zespołowy skończony przed terminem! Otrzymujesz 150 Złotych Rud nagrody."), 2));
    ChanceCards.Add(FChanceCard(TEXT("Zebrałeś ekipę projektową! Otrzymujesz 50 ZR od każdego gracza."), 3));
    ChanceCards.Add(FChanceCard(TEXT("Masz szczęście! Dziekanat przyznał Ci jednorazowy immunitet przed Zamkniętym Laboratorium."), 4));
    ChanceCards.Add(FChanceCard(TEXT("Zajęcia odwołane! Przenieś się na pole START i pobierz 200 Złotych Rud."), 5));
    ChanceCards.Add(FChanceCard(TEXT("Stypendium rektora za wyniki w nauce! Otrzymujesz 300 Złotych Rud."), 6));
    ChanceCards.Add(FChanceCard(TEXT("Udało Ci się znaleźć miejsce w Akwarium! Odpocznij – pomiń następną turę i otrzymujesz 50 Złotych Rud."), 7));
    ChanceCards.Add(FChanceCard(TEXT("Dodatkowy punkt z projektu! Otrzymujesz 50 Złotych Rud za zaangażowanie."), 8));
    ChanceCards.Add(FChanceCard(TEXT("Kolokwium niespodzianka! Płać 100 Złotych Rud uczelni."), 9));
    ChanceCards.Add(FChanceCard(TEXT("Zgubiłeś legitymację! Płać 150 Złotych Rud za duplikat."), 10));
    ChanceCards.Add(FChanceCard(TEXT("Awaria USOS'a! Czekasz jedną kolejkę – nie możesz zapisać się na zajęcia."), 11));
    ChanceCards.Add(FChanceCard(TEXT("Brak miejsca w Akwarium! Idziesz prosto do Zablokowanego Laboratorium."), 12));
    ChanceCards.Add(FChanceCard(TEXT("Nie oddałeś sprawozdania na czas! Płać 100 Złotych Rud kary."), 13));
    ChanceCards.Add(FChanceCard(TEXT("Wezwanie do biblioteki! Przenieś się na najbliższą bibliotekę. Jeśli jest czyjaś – zapłać czynsz."), 14));
    ChanceCards.Add(FChanceCard(TEXT("Akcja czysty wydział! Każdy gracz wpłaca 20 Złotych Rud do uczelni."), 15));

    if (CachedHubWidget) CachedHubWidget->SetRollButtonVisible(false);

    // Find all the squares on the board
    TArray<AActor*> FoundTiles;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AABoardTile::StaticClass(), FoundTiles);

    for (AActor* A : FoundTiles) {
        if (AABoardTile* Tile = Cast<AABoardTile>(A)) BoardTiles.Add(Tile);
    }

    StartGameSetup(); // player initialization

    // HUD initialization
    if (UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance())) {
        APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
        if (PC) {
            GI->InitializeHUD(PC);
            CachedHubWidget = GI->HUDWidgetInstance;

            UE_LOG(LogTemp, Log, TEXT("HUD został utworzony przez GameMode."));

            // Setting UI-only mode
            FInputModeUIOnly InputMode;
            InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
            PC->SetInputMode(InputMode);
            PC->bShowMouseCursor = true;
        }
    }

    // Set the starting turn and show it in the HUD
    if (CachedHubWidget && PlayerCharacters.Num() > 0) {
        CurrentPlayerID = FMath::RandRange(1, PlayerCharacters.Num());

        UE_LOG(LogTemp, Warning, TEXT("Player %d was randomly selected to start the game."), CurrentPlayerID);

        APlayerCharacter* CurrentPlayer = GetPlayerByID(CurrentPlayerID);
        if (CurrentPlayer && CachedHubWidget) {
            FString TurnText = FString::Printf(TEXT("Player turn: %s"), *CurrentPlayer->GetPlayerName());
            CachedHubWidget->UpdateTurn(TurnText);
        }

        // Point the camera at this player
        FocusCameraOnCurrentPlayer();
    }

    // Sorting by TileID
    BoardTiles.Sort([](const AABoardTile& A, const AABoardTile& B) { return A.TileID < B.TileID; });

    // Camera
    FocusCameraOnCurrentPlayer();

    // Update balance and number of fields + connection to bp fields type 1 and 2
    if (CachedHubWidget) {
        for (APlayerCharacter* PC : PlayerCharacters) {
            if (PC) {
                CachedHubWidget->UpdateMoney(PC->PlayerID, PC->Money);
                CachedHubWidget->UpdateOwnedTiles(PC->PlayerID, PC->OwnedTileCount);

                // Upgrading Special Fields 1 and 2
                PC->RebuildOwnedTilesFromBoard();
            }
        }
    }

    // Find physical dices in the world
    TArray<AActor*> FoundDice;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADiceActor::StaticClass(), FoundDice);

    if (FoundDice.Num() >= 1) Dice1Ref = Cast<ADiceActor>(FoundDice[0]);

    if (FoundDice.Num() >= 2) Dice2Ref = Cast<ADiceActor>(FoundDice[1]);

    // Find a throw camera in the world
    TArray<AActor*> FoundCameras;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACameraActor::StaticClass(), FoundCameras);

    for (AActor* Cam : FoundCameras) {
        if (Cam->GetName().Contains("BP_DiceCamera")) {
            DiceCamera = Cast<ACameraActor>(Cam);
            UE_LOG(LogTemp, Warning, TEXT("Throw camera found: %s"), *Cam->GetName());
            break;
        }
    }
}

void AMyGameMode::StartGameSetup()
{
    // Find all players in the world
    TArray<AActor*> FoundPlayers;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerCharacter::StaticClass(), FoundPlayers);

    PlayerCharacters.Empty();

    // Sort by name to have a fixed order - no matter what order UE5 is found in
    FoundPlayers.Sort([](const AActor& A, const AActor& B) { return A.GetName() < B.GetName(); });

    // Set PlayerID
    int32 IDCounter = 1;
    for (AActor* A : FoundPlayers) {
        if (APlayerCharacter* PC = Cast<APlayerCharacter>(A)) {
            PC->PlayerID = IDCounter;
            IDCounter++;
            PlayerCharacters.Add(PC);
        }
    }

    // Set names after ID - from widget
    if (UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance())) {
        for (APlayerCharacter* PC : PlayerCharacters) {
            switch (PC->PlayerID) {
                case 1: PC->SetPlayerName(GI->Player1Name); break;
                case 2: PC->SetPlayerName(GI->Player2Name); break;
                case 3: PC->SetPlayerName(GI->Player3Name); break;
                case 4: PC->SetPlayerName(GI->Player4Name); break;
            }
        }
    }
}

void AMyGameMode::NextTurn()
{
    // Remember previous player
    int32 PreviousID = CurrentPlayerID;

    // Go to the next player
    CurrentPlayerID++;
    if (CurrentPlayerID > PlayerCharacters.Num())
        CurrentPlayerID = 1;

    // Ignore players who went bankrupt
    APlayerCharacter* NextPlayer = GetPlayerByID(CurrentPlayerID);
    while (NextPlayer && NextPlayer->bIsBankrupt) {
        CurrentPlayerID++;
        if (CurrentPlayerID > PlayerCharacters.Num())
            CurrentPlayerID = 1;

        NextPlayer = GetPlayerByID(CurrentPlayerID);
    }

    // Get the current player (the one who has a turn now)
    APlayerCharacter* CurrentPlayer = GetPlayerByID(CurrentPlayerID);
    if (!CurrentPlayer) return;

    // Checking if a player is in jail
    if (CurrentPlayer->TurnsInJail > 0) {
        CurrentPlayer->TurnsInJail--;

        if (CachedHubWidget) {
            FString JailText = FString::Printf(TEXT("Gracz %s jest w więzieniu. Zostało %d tur."), *CurrentPlayer->GetPlayerName(), CurrentPlayer->TurnsInJail);
            CachedHubWidget->UpdateTurn(JailText);
        }

        NextTurn();
        return;
    }

    // Skip turn
    if (CurrentPlayer->TurnsToSkip){
        CurrentPlayer->TurnsToSkip = 0;

        if (CachedHubWidget) {
            FString Msg = FString::Printf(TEXT("%s opuszcza turę!"), *CurrentPlayer->PlayerName);
            CachedHubWidget->ShowMessage(Msg);
        }

        NextTurn();
        return;
    }

    FString TurnText = FString::Printf(TEXT("Player turn: %s"), *CurrentPlayer->GetPlayerName());
    if (CachedHubWidget)
        CachedHubWidget->UpdateTurn(TurnText);

    // Set the camera to the new player
    FocusCameraOnCurrentPlayer();

    // After a turn change - set the HUD buttons
    if (CachedHubWidget) {
        CachedHubWidget->SetRollButtonVisible(true);        
        CachedHubWidget->SetNextTurnButtonVisible(false);   
        CachedHubWidget->SetBuyButtonVisible(false);        
        CachedHubWidget->SetMortgageButtonVisible(false);
        CachedHubWidget->SetRedeemButtonVisible(false);
        CachedHubWidget->SetUpgradeButtonVisible(false);
    }
}

void AMyGameMode::RollDice()
{
    if (bIsRolling) return;

    if (CachedHubWidget) {
        CachedHubWidget->SetRollButtonVisible(false);
        CachedHubWidget->SetUpgradeButtonVisible(false);
        CachedHubWidget->SetMortgageButtonVisible(false);
        CachedHubWidget->SetRedeemButtonVisible(false);
        CachedHubWidget->SetBuyButtonVisible(false);
    }

    APlayerCharacter* CurrentPlayer = GetPlayerByID(CurrentPlayerID);
    if (!CurrentPlayer) return;

    // Switch camera to dices
    FocusCameraOnDice();

    // Set the flag
    bIsRolling = true;
    DiceStoppedCount = 0;
    Dice1Value = 0;
    Dice2Value = 0;

    FTimerHandle RollDelayHandle;
    GetWorld()->GetTimerManager().SetTimer(RollDelayHandle, [this]() {
            if (DiceRollSound) UGameplayStatics::PlaySound2D(this, DiceRollSound);

            if (!Dice1Ref || !Dice2Ref) {
                // emergency roll if no physical dice
                int32 Dice1 = FMath::RandRange(1, 6);
                int32 Dice2 = FMath::RandRange(1, 6);

                if (CachedHubWidget)
                    CachedHubWidget->ShowDiceResult(Dice1, Dice2);

                HandleDiceResult(Dice1, Dice2);
                bIsRolling = false;
                return;
            }

            // Position and speed reset
            Dice1Ref->SetActorLocation(Dice1StartLocation);
            Dice1Ref->SetActorRotation(FRotator::ZeroRotator);
            Dice1Ref->DiceMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
            Dice1Ref->DiceMesh->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);

            Dice2Ref->SetActorLocation(Dice2StartLocation);
            Dice2Ref->SetActorRotation(FRotator::ZeroRotator);
            Dice2Ref->DiceMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
            Dice2Ref->DiceMesh->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);

            // Link events.
            if (Dice1Ref)
                Dice1Ref->OnDiceStopped.AddDynamic(this, &AMyGameMode::OnDiceStoppedHandler);

            if (Dice2Ref) 
                Dice2Ref->OnDiceStopped.AddDynamic(this, &AMyGameMode::OnDiceStoppedHandler);

            Dice1Ref->RollDice();
            Dice2Ref->RollDice();

        }, 1.f, false);
}

void AMyGameMode::OnDiceStoppedHandler()
{
    if (!bIsRolling) return;

    DiceStoppedCount++;

    if (DiceStoppedCount < 2) return;

    bIsRolling = false;

    FTimerHandle DelayBeforeResultHandle;
    GetWorld()->GetTimerManager().SetTimer(DelayBeforeResultHandle, [this]() { 
            Dice1Value = Dice1Ref ? Dice1Ref->GetDiceResult() : FMath::RandRange(1, 6);
            Dice2Value = Dice2Ref ? Dice2Ref->GetDiceResult() : FMath::RandRange(1, 6);

            int32 Sum = Dice1Value + Dice2Value;

            if (CachedHubWidget) CachedHubWidget->ShowDiceResult(Dice1Value, Dice2Value);

            FocusCameraOnCurrentPlayer();

            FTimerHandle MoveDelayHandle;
            GetWorld()->GetTimerManager().SetTimer(MoveDelayHandle, [this]() {
                    HandleDiceResult(Dice1Value, Dice2Value);

                }, 0.5f, false);

        }, 1.0f, false);

    // Removing unnecessary events
    if (Dice1Ref)
        Dice1Ref->OnDiceStopped.RemoveDynamic(this, &AMyGameMode::OnDiceStoppedHandler);
    if (Dice2Ref)
        Dice2Ref->OnDiceStopped.RemoveDynamic(this, &AMyGameMode::OnDiceStoppedHandler);
}

void AMyGameMode::HandleDiceResult(int32 Dice1, int32 Dice2)
{
    int32 Steps = Dice1 + Dice2;

    APlayerCharacter* CurrentPlayer = GetPlayerByID(CurrentPlayerID);
    if (!CurrentPlayer)
        return;

    if (Dice1 == Dice2) {
        CurrentPlayer->ConsecutiveDoubles++;

        if (CurrentPlayer->ConsecutiveDoubles >= 3) {
            if (CachedHubWidget) {
                CachedHubWidget->ShowMessage(TEXT("Trzy razy double, idziesz do zamkniętego laboratorium..."));
                CachedHubWidget->SetRollButtonVisible(false); // zablokuj rzut w trakcie animacji
            }

            FTimerHandle JailDelayHandle;
            GetWorld()->GetTimerManager().SetTimer(JailDelayHandle, [this, CurrentPlayer]()
                {
                    CurrentPlayer->SendToJail();

                    NextTurn();

                    if (CachedHubWidget)
                        CachedHubWidget->SetRollButtonVisible(true);

                }, 2.0f, false);

            return;
        } else {
            //Double -> extra throw
            bExtraRollGranted = true;
            if (CachedHubWidget)
            {
                FString Msg = FString::Printf(TEXT("Wyrzucono: %d i %d - masz dodatkowy rzut!"), Dice1, Dice2);
                CachedHubWidget->ShowMessage(Msg);
            }
        }
    } else {
        // No double
        CurrentPlayer->ConsecutiveDoubles = 0;
        bExtraRollGranted = false;

        if (CachedHubWidget) CachedHubWidget->SetRollButtonVisible(false);

    }

    // Player movement
    bIsPlayerMoving = true;
    CurrentPlayer->MoveBySteps(Steps);
}

void AMyGameMode::FocusCameraOnCurrentPlayer()
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PC) return;

    APlayerCharacter* CurrentPlayer = GetPlayerByID(CurrentPlayerID);
    if (!CurrentPlayer || !CurrentPlayer->PlayerCamera) return;

    PC->SetViewTargetWithBlend(CurrentPlayer, 0.5f, EViewTargetBlendFunction::VTBlend_Cubic);
}

void AMyGameMode::FocusCameraOnDice()
{
    if (!DiceCamera) return;

    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PC || !PC->PlayerCameraManager) return;

    PC->SetViewTarget(DiceCamera);
}

void AMyGameMode::AssignBankruptcyRank(APlayerCharacter* BankruptPlayer)
{
    if (!BankruptPlayer) return;

    BankruptcyCounter++;

    int32 Rank = PlayerCharacters.Num() - BankruptcyCounter + 1;
    BankruptPlayer->FinalRank = Rank;

    // Check how many players are left
    int32 ActivePlayers = 0;
    APlayerCharacter* LastPlayer = nullptr;

    for (APlayerCharacter* PC : PlayerCharacters) {
        if (PC && !PC->bIsBankrupt) {
            ActivePlayers++;
            LastPlayer = PC;
        }
    }

    // If only one player remains, he wins.
    if (ActivePlayers == 1 && LastPlayer){
        LastPlayer->FinalRank = 1;

        // Calculate his final result (wealth)
        LastPlayer->Score = LastPlayer->Money;
        for (AABoardTile* Tile : BoardTiles){
            if (Tile && Tile->OwnerID == LastPlayer->PlayerID){
                LastPlayer->Score += Tile->TileCost;
                if (Tile->UpgradeLevel > 0)
                    LastPlayer->Score += Tile->UpgradeLevel * (Tile->TileCost * 0.25f);
            }
        }
    }
}

void AMyGameMode::ShowFinalRankingWidget(float Delay)
{
    FTimerHandle FinalWidgetTimer;
    GetWorld()->GetTimerManager().SetTimer(FinalWidgetTimer, [this]() {
            if (UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance())) {
                GI->FinalPlayerNames.Empty();
                GI->FinalScores.Empty();
                GI->FinalRanks.Empty();

                // Sorting players
                PlayerCharacters.Sort([](const APlayerCharacter& A, const APlayerCharacter& B) { return A.FinalRank < B.FinalRank; });

                // Save data to GameInstance
                for (APlayerCharacter* PC : PlayerCharacters) {
                    if (!PC) continue;

                    GI->FinalPlayerNames.Add(PC->GetPlayerName());
                    GI->FinalScores.Add(PC->Score);
                    GI->FinalRanks.Add(PC->FinalRank);
                }

                GI->RemoveHUD();
            }

            UGameplayStatics::OpenLevel(this, FName("L_FinalMenu_V01"));

        }, Delay, false);
}

// Chance card drawing function
void AMyGameMode::TriggerChanceCard(int32 PlayerID)
{
    if (!CachedHubWidget)
        return;

    if (ChanceCards.Num() == 0) {
        UE_LOG(LogTemp, Error, TEXT("Brak kart szansy!"));
        return;
    }

    int32 Index = FMath::RandRange(0, ChanceCards.Num() - 1);
    const FChanceCard& Card = ChanceCards[Index];

    PendingChanceEffect = Card.EffectID;
    PendingChancePlayerID = PlayerID;

    CachedHubWidget->ShowChanceCard(FString::Printf(TEXT("Opis karty:\n%s"), *Card.Description));
}

// How chance cards work
void AMyGameMode::ApplyPendingChanceEffect()
{
    if (PendingChanceEffect < 0 || PendingChancePlayerID < 0)
        return;

    APlayerCharacter* PC = GetPlayerByID(PendingChancePlayerID);
    if (!PC)
        return;

    int32 Effect = PendingChanceEffect;
    PendingChanceEffect = -1;
    PendingChancePlayerID = -1;

    switch (Effect) {
        case 1: PC->AddMoney(200);  break;                
        case 2:  PC->AddMoney(150);  break;                
        case 3: 
            {
                int32 TotalReceived = 0;

                for (APlayerCharacter* Other : PlayerCharacters) {
                    if (!Other || Other == PC || Other->bIsBankrupt)
                        continue;

                    int32 Payment = 50;

                    int32 MoneyAvailable = Other->Money;
                    int32 ActualPayment = FMath::Min(Payment, MoneyAvailable);

                    Other->RemoveMoney(ActualPayment);

                    TotalReceived += ActualPayment;
                }

                PC->AddMoney(TotalReceived);

                if (CachedHubWidget) {
                    FString Msg = FString::Printf(TEXT("Otrzymujesz %d ZR."), TotalReceived);
                    CachedHubWidget->ShowMessage(Msg);
                }

                break;
            }
        case 4:  PC->bHasJailPass = true; break;               
        case 5:  PC->SendToStart(); break;          
        case 6:  PC->AddMoney(300);  break;                
        case 7:
            {
                PC->AddMoney(50);

                if (bExtraRollGranted) {
                    bExtraRollGranted = false;
                    PC->ConsecutiveDoubles = 0;

                    if (CachedHubWidget)
                        CachedHubWidget->ShowMessage(TEXT("Akwarium: Twój dodatkowy rzut anuluje czekaniae! Koniec tury."));
                }
                else {
                    PC->TurnsToSkip = 1;
                    if (CachedHubWidget)
                        CachedHubWidget->ShowMessage(TEXT("Akwarium: Odpocznij - tracisz następną kolejkę."));
                }

                if (CachedHubWidget) {
                    CachedHubWidget->SetRollButtonVisible(false);
                    CachedHubWidget->SetNextTurnButtonVisible(true);
                }

                break;
            }
        case 8: PC->AddMoney(50); break;                 
        case 9: PC->RemoveMoney(100); break;           
        case 10: PC->RemoveMoney(150); break;           
        case 11:  PC->TurnsToSkip = 1; break;
        case 12: 
            {
                bExtraRollGranted = false; //if double

                PC->SendToJail(); break;

                if (CachedHubWidget) {
                    CachedHubWidget->SetRollButtonVisible(false);
                    CachedHubWidget->SetNextTurnButtonVisible(true);
                }
            }
        case 13: PC->RemoveMoney(100); break;           
        case 14: PC->SendToLibrary(); break;
        case 15:
            {
                for (APlayerCharacter* P : PlayerCharacters) {
                    if (!P || P->bIsBankrupt)
                        continue;

                    int32 Payment = 20;
                    int32 MoneyAvailable = P->Money;
                    int32 ActualPayment = FMath::Min(Payment, MoneyAvailable);

                    P->RemoveMoney(ActualPayment);
                }

                if (CachedHubWidget)
                    CachedHubWidget->ShowMessage(TEXT("Każdy gracz płaci 20 ZR!"));

                break;
            }
    }

    UE_LOG(LogTemp, Warning, TEXT("[CHANCE] effect performed: %d"), Effect);
}

