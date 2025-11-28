#include "PlayerCharacter.h"
#include "ABoardTile.h"
#include "MyGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "HubMainWidget.h"

APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Camera configuration 
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	
	// DISTANCE of the camera from the player
	SpringArm->TargetArmLength = 300.f;

	// RAISE the camera relative to the pawn
	SpringArm->SetRelativeLocation(FVector(0.f, 0.f, 350.f));

	// Downward viewing angle
	SpringArm->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));

	// Prevents the camera from bumping into other objects
	SpringArm->bDoCollisionTest = true;

	SpringArm->bUsePawnControlRotation = false;

	PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
	PlayerCamera->SetupAttachment(SpringArm);
	PlayerCamera->bUsePawnControlRotation = false;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->Deactivate();

    // Set soft collision (no pushing)
    UCapsuleComponent* Capsule = GetCapsuleComponent();
    Capsule->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Capsule->SetCollisionResponseToAllChannels(ECR_Ignore);
    Capsule->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);

    // Set player height to start
    FVector CurrentLoc = GetActorLocation();
    CurrentLoc.Z = 41.5f; 
    SetActorLocation(CurrentLoc);
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void APlayerCharacter::AddMoney(int32 Amount)
{
	Money += Amount;

    if (GetWorld()) {
        AMyGameMode* GM = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
            if (GM && GM->CachedHubWidget)
                GM->CachedHubWidget->UpdateMoney(PlayerID, Money);
    }
    
}

void APlayerCharacter::RemoveMoney(int32 Amount)
{
    if (Amount <= 0)
        return;

    if (Money < Amount) {
        Money = 0;

        if (GetWorld()) {
            AMyGameMode* GM = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
            if (GM && GM->CachedHubWidget)
                GM->CachedHubWidget->UpdateMoney(PlayerID, Money);
        }

        DeclareBankruptcy();
        return;
    }

    Money -= Amount;

    if (GetWorld()) {
    AMyGameMode* GM = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
        if (GM && GM->CachedHubWidget)
            GM->CachedHubWidget->UpdateMoney(PlayerID, Money);
    }
}

void APlayerCharacter::AddOwnedTile(AABoardTile* Tile)
{
    if (!Tile) return;

    if (!OwnedTiles.Contains(Tile)) {
        OwnedTiles.Add(Tile);
        OwnedTileCount = OwnedTiles.Num();

        if (UWorld* World = GetWorld()) {
            AMyGameMode* GM = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(World));
            if (GM && GM->CachedHubWidget) {
                GM->CachedHubWidget->UpdateOwnedTiles(PlayerID, OwnedTileCount);
            }
        }

        UpdateRentBonus();
    }
}

// Set player name
void APlayerCharacter::SetPlayerName(const FString& NewName)
{
	PlayerName = NewName;
}

// Get player name
FString APlayerCharacter::GetPlayerName() const
{
	return PlayerName;
}

void APlayerCharacter::MoveBySteps(int32 Steps)
{
	if (Steps <= 0)
		return;

	AMyGameMode* GM = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!GM || GM->BoardTiles.Num() == 0) {
		return;
	}

	RemainingSteps = Steps;

    // Motion animation
    PlayMoveAnimation();

	GetWorld()->GetTimerManager().ClearTimer(MoveTimerHandle);

	MoveToNextTile(GM);
}

void APlayerCharacter::MoveToNextTile(AMyGameMode* GM)
{
    if (!GM || GM->BoardTiles.Num() == 0)
        return;

    // End of movement
    if (RemainingSteps <= 0) {
        GetWorld()->GetTimerManager().ClearTimer(MoveTimerHandle);
        PlayIdleAnimation();

        AABoardTile* CurrentTile = GM->BoardTiles[CurrentTileIndex];
        if (CurrentTile) {
            // Laboratory tour
            if (CurrentTile->TileID == 10) {
                if (GM->CachedHubWidget)
                    GM->CachedHubWidget->ShowMessage(TEXT("Zwiedzasz zamknięte laboratorium :>"));
            }

            // Field of Chance
            if (CurrentTile->TileID == 8 || CurrentTile->TileID == 23 || CurrentTile->TileID == 32) {
                if (ChanceCardSound)
                    UGameplayStatics::PlaySound2D(this, ChanceCardSound);
                if (GM)
                    GM->TriggerChanceCard(PlayerID);
            }

            // Getting to a closed laboratory (field 30)
            if (CurrentTile->TileID == 30) {
                if(OnLandSound)
                    UGameplayStatics::PlaySound2D(this, OnLandSound);

                if (bHasJailPass) {
                    bHasJailPass = false;
                    if (GM->CachedHubWidget)
                    {
                        FString Msg = FString::Printf(TEXT("%s używa przepustki z Dziekanatu i unika Zamkniętego Laboratorium!"), *PlayerName);
                        GM->CachedHubWidget->ShowMessage(Msg);
                    }
                }
                else {
                    if (GM->CachedHubWidget)
                        GM->CachedHubWidget->ShowMessage(TEXT("Trafiasz do zamkniętego laboratorium, tracisz 3 rundy."));

                    SendToJail();

                    if (GM->CachedHubWidget) {
                        GM->CachedHubWidget->SetRollButtonVisible(false);
                        GM->CachedHubWidget->SetNextTurnButtonVisible(true);
                        GM->CachedHubWidget->SetBuyButtonVisible(false);
                        GM->CachedHubWidget->SetMortgageButtonVisible(false);
                        GM->CachedHubWidget->SetRedeemButtonVisible(false);
                        GM->CachedHubWidget->SetUpgradeButtonVisible(false);
                    }

                    return;
                }
            }

            // Tax fields
            if (CurrentTile->TileID == 6 || CurrentTile->TileID == 17 || CurrentTile->TileID == 38) {
                int32 TaxAmount = CurrentTile->TileCost;
                RemoveMoney(TaxAmount);

                FString TaxName = (CurrentTile->TileID == 6) ? TEXT("Podatek studencki") : TEXT("Podatek ekstremalny");

                // If a player goes bankrupt
                if (Money <= 0) {
                    DeclareBankruptcy();
                    return; 
                }

                // If the player has not gone bankrupt
                if (GM->CachedHubWidget) {
                    GM->CachedHubWidget->UpdateMoney(PlayerID, Money);

                    FString Msg = FString::Printf(TEXT("%s - %d ZR"), *TaxName, TaxAmount);

                    GM->CachedHubWidget->ShowMessage(Msg);

                    if (TaxSound) UGameplayStatics::PlaySound2D(this, TaxSound, 0.8f);
                }
            }

            // Hiding the buy button
            if (GM->CachedHubWidget) GM->CachedHubWidget->SetBuyButtonVisible(false);

            // Checking if a field belongs to another player
            if (CurrentTile->OwnerID != PlayerID && CurrentTile->OwnerID != -1) {
                // If the field is mortgaged, DO NOT collect rent
                if (CurrentTile->bIsMortgaged){
                    if (GM->CachedHubWidget) {
                        FString Msg = FString::Printf(TEXT("To pole jest zastawione, nie ponosisz kosztu czynszu."));
                        GM->CachedHubWidget->ShowMessage(Msg);
                    }
                }else{
                    int Rent = CurrentTile->GetUpgradedRent();

                    APlayerCharacter* OwnerPlayer = GM->GetPlayerByID(CurrentTile->OwnerID);
                    if (OwnerPlayer) {
                        int32 OldMoney = Money; // Remember before paying

                        RemoveMoney(Rent);

                        if (bIsBankrupt) {
                            FTimerHandle RentMessageDelay;
                            GetWorld()->GetTimerManager().SetTimer(RentMessageDelay, [GM, OwnerPlayer, OldMoney, this]() {
                                    if (OldMoney > 0) {
                                        FString RentMsg = FString::Printf(TEXT("Dajesz %d ZR graczowi %s"), OldMoney, *OwnerPlayer->GetPlayerName());
                                        if (GM && GM->CachedHubWidget) GM->CachedHubWidget->ShowMessage(RentMsg);

                                        OwnerPlayer->AddMoney(OldMoney);

                                        if (TaxSound) UGameplayStatics::PlaySound2D(this, TaxSound, 0.8f);
                                    }
                                }, 2.0f, false);
                            return;
                        }

                        OwnerPlayer->AddMoney(Rent);

                        if (GM->CachedHubWidget) {
                            GM->CachedHubWidget->UpdateMoney(PlayerID, Money);
                            GM->CachedHubWidget->UpdateMoney(OwnerPlayer->PlayerID, OwnerPlayer->Money);

                            FString RentMsg = FString::Printf(TEXT("Dajesz %d ZR graczowi %s"), Rent, *OwnerPlayer->GetPlayerName());
                            GM->CachedHubWidget->ShowMessage(RentMsg);

                            if (TaxSound) UGameplayStatics::PlaySound2D(this, TaxSound, 0.8f);
                        }
                    }
                }
            } else if (CurrentTile->bCanBeBought && CurrentTile->OwnerID == -1){    // Field available for purchase
                if (GM->CachedHubWidget) GM->CachedHubWidget->SetBuyButtonVisible(true);
            }
        } 
        
        // Pledge/purchase of a field if we are the owner + improvement
        if (CurrentTile->OwnerID == PlayerID) {         
            if (GM->CachedHubWidget) {
                // When the field is not a special field and is not blocked
                if (CurrentTile->SpecialType == 0 && !CurrentTile->bIsMortgaged) {
                    // Can the field be further improved?
                    bool bCanUpgrade = (CurrentTile->UpgradeLevel + 1 < CurrentTile->UpgradeIncomeBonuses.Num());

                    // Set button visibility depending on the result
                    GM->CachedHubWidget->SetUpgradeButtonVisible(bCanUpgrade);

                } else {
                    // Hide button for special fields (type 1 or 2) or blocked fields
                    GM->CachedHubWidget->SetUpgradeButtonVisible(false);
                }

                // If the field is pledged - show the "Buy field" button
                if (CurrentTile->bIsMortgaged) {
                    GM->CachedHubWidget->SetRedeemButtonVisible(true);
                    GM->CachedHubWidget->SetMortgageButtonVisible(false);
                } else // pole własne i nie zastawione — pokaż przycisk "Zastaw pole"
                {
                    GM->CachedHubWidget->SetMortgageButtonVisible(true);
                    GM->CachedHubWidget->SetRedeemButtonVisible(false);
                }
            }
        } else {
            // Not your own field - hide both
            if (GM->CachedHubWidget) {
                GM->CachedHubWidget->SetMortgageButtonVisible(false);
                GM->CachedHubWidget->SetRedeemButtonVisible(false);

                // Hide upgrade field
                GM->CachedHubWidget->SetUpgradeButtonVisible(false);
            }
        }

        // End of movement
        GM->bIsPlayerMoving = false;

        // HUD – end of turn or extra roll
        if (!GM->bExtraRollGranted) {
            // Normal end of turn
            if (GM->CachedHubWidget)
                GM->CachedHubWidget->SetNextTurnButtonVisible(true);
        } else { // Extra throw
            GM->bExtraRollGranted = false; // reset flag after showing button
            if (GM->CachedHubWidget) GM->CachedHubWidget->SetRollButtonVisible(true);
        }

        return;
    }

    // Next step
    RemainingSteps--;
    CurrentTileIndex = (CurrentTileIndex + 1) % GM->BoardTiles.Num();

    // Passing through START
    if (CurrentTileIndex == 0) {
        AABoardTile* StartTile = GM->BoardTiles[0];

        if (StartTile) {
            int32 Reward = StartTile->TileCost;
            AddMoney(Reward);

            if(StartTileSound) UGameplayStatics::PlaySound2D(this, StartTileSound, 0.5f);

            if (GM->CachedHubWidget) {
                FString Msg = FString::Printf(TEXT("Otrzymujesz %d ZR!"), Reward);
                GM->CachedHubWidget->ShowMessage(Msg);
            }
        }
    }

    // Character movement (step animation)
    AABoardTile* TargetTile = GM->BoardTiles[CurrentTileIndex];
    if (!TargetTile) return;

    FVector Start = GetActorLocation();
    FVector End = TargetTile->GetActorLocation();
    End.Z = 41.5f;

    // Moving the pieces so that they do not overlap
    const float ForwardDistance = 15.f;
    const float SideDistance = 10.f;
    FVector Offset(0.f);

    switch (PlayerID){
        case 1: Offset = FVector(ForwardDistance, -SideDistance, 0.f); break;
        case 2: Offset = FVector(ForwardDistance, SideDistance, 0.f); break;
        case 3: Offset = FVector(-ForwardDistance, -SideDistance, 0.f); break;
        case 4: Offset = FVector(-ForwardDistance, SideDistance, 0.f); break;
    }

    End += Offset;

    // Rotating characters on the corners of the board
    if (CurrentTileIndex == 0 || CurrentTileIndex == 10 || CurrentTileIndex == 20 || CurrentTileIndex == 30) {
        if (GetMesh()) {
            FRotator MeshRot = GetMesh()->GetComponentRotation();
            MeshRot.Yaw += 90.f;
            GetMesh()->SetWorldRotation(MeshRot);
        }

        if (SpringArm) {
            FRotator ArmRot = SpringArm->GetRelativeRotation();
            ArmRot.Yaw += 90.f;
            SpringArm->SetRelativeRotation(ArmRot);
        }
    }

    // Animation of movement on the board
    ElapsedTime = 0.f;
    const float MoveDuration = 0.3f;
    const float MoveInterval = 0.01f;

    GetWorld()->GetTimerManager().SetTimer( MoveTimerHandle, [this, GM, Start, End, MoveDuration]() mutable {
            ElapsedTime += 0.01f;
            float Alpha = FMath::Clamp(ElapsedTime / MoveDuration, 0.f, 1.f);

            FVector NewLocation = FMath::Lerp(Start, End, Alpha);
            NewLocation.Z = 41.5f;
            SetActorLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);

            if (Alpha >= 1.f){
                GetWorld()->GetTimerManager().ClearTimer(MoveTimerHandle);
                // Next step after a short delay
                FTimerHandle DelayHandle;
                GetWorld()->GetTimerManager().SetTimer(DelayHandle, [this, GM]() { MoveToNextTile(GM); }, 0.1f, false);
            }
        }, MoveInterval, true );
}

void APlayerCharacter::PlayMoveAnimation()
{
    if (MoveAnimation){
        GetMesh()->PlayAnimation(MoveAnimation, true);
        GetMesh()->SetPlayRate(1.0f);
    }
}

void APlayerCharacter::PlayIdleAnimation()
{
    if (IdleAnimation){
        GetMesh()->PlayAnimation(IdleAnimation, true);
        GetMesh()->SetPlayRate(1.0f);
    }
}

// Pledge and redemption of fields
void APlayerCharacter::MortgageTile()
{
    AMyGameMode* GM = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
    if (!GM) return;

    // Find the current field
    if (GM->BoardTiles.IsValidIndex(CurrentTileIndex)) {
        AABoardTile* CurrentTile = GM->BoardTiles[CurrentTileIndex];
        // Calculate the value of the pledge
        float MortgageValue = CurrentTile->GetMortgageValue();

        // Add money
        AddMoney(MortgageValue);

        // Mark the field as pledged
        CurrentTile->bIsMortgaged = true;

        if (GM->CachedHubWidget){
            GM->CachedHubWidget->UpdateMoney(PlayerID, Money);

            FString Msg = FString::Printf(TEXT("Zastawiles pole za %.0f ZR."), MortgageValue);
            GM->CachedHubWidget->ShowMessage(Msg);
        }
    }
}

void APlayerCharacter::RedeemTile()
{
    AMyGameMode* GM = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
    if (!GM) return;

    if (GM->BoardTiles.IsValidIndex(CurrentTileIndex))
    {
        AABoardTile* CurrentTile = GM->BoardTiles[CurrentTileIndex];
        // Calculate the cost of redemption (borrowed amount + 10% interest)
        float RedeemCost = CurrentTile->GetRedeemCost();

        if (Money < RedeemCost)
        {
            if (GM->CachedHubWidget)
                GM->CachedHubWidget->ShowMessage(TEXT("Nie masz wystarczająco pieniedzy."));
            return;
        }

        // Pay for redemption
        RemoveMoney(RedeemCost);

        // Mark the field as purchased
        CurrentTile->bIsMortgaged = false;

        if (GM->CachedHubWidget)
        {
            GM->CachedHubWidget->UpdateMoney(PlayerID, Money);

            FString Msg = FString::Printf(TEXT("Odzyskales pole za %.0f ZR."), RedeemCost);
            GM->CachedHubWidget->ShowMessage(Msg);
        }
    }
}

// Bonuses for special fields type 1 and 2
// Function for testing purposes - test by bp whether bonuses work
void APlayerCharacter::RebuildOwnedTilesFromBoard()
{
    AMyGameMode* GM = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
    if (!GM) return;

    OwnedTiles.Empty();

    for (AABoardTile* Tile : GM->BoardTiles)
    {
        if (!Tile) continue;

        if (Tile->OwnerID == PlayerID)
        {
            OwnedTiles.Add(Tile);
        }
    }

    OwnedTileCount = OwnedTiles.Num();

    UpdateRentBonus();
}

void APlayerCharacter::UpdateRentBonus()
{
    int32 CountType1 = 0;
    int32 CountType2 = 0;

    // We count the number of special fields from the table of the player's fields.
    for (AABoardTile* Tile : OwnedTiles) {
        if (!Tile) continue;

        if (Tile->SpecialType == 1) CountType1++;
        if (Tile->SpecialType == 2) CountType2++;
    }

    RentBonusType1 = 1.0f;
    RentBonusType2 = 1.0f;

    // Type 1
    if (CountType1 == 3)
        RentBonusType1 = 1.10f;

    // Type 2
    switch (CountType2){
        case 2: RentBonusType2 = 1.05f; break;
        case 3: RentBonusType2 = 1.12f; break;
        case 4: RentBonusType2 = 1.20f; break;
    }

}

// Bankrupcity
void APlayerCharacter::DeclareBankruptcy()
{
    if (bIsBankrupt) return; // already bankrupt

    bIsBankrupt = true;

    // Calculate the value of your fields
    Score = 0;
    for (AABoardTile* Tile : OwnedTiles) {
        if (!Tile) continue;

        // Podstawowa wartość pola
        Score += Tile->TileCost;

        // Additional value for upgrades
        if (Tile->UpgradeLevel > 0 && Tile->UpgradeIncomeBonuses.Num() > 0) {
            int32 BonusValue = Tile->UpgradeLevel * (Tile->TileCost * 0.25f);
            Score += BonusValue;
        }

        // If the field was blocked, deduct half of the value.
        if (Tile->bIsMortgaged) {
            Score -= Tile->TileCost * 0.5f;
        }
    }

    // Add cash to your score
    Score += Money;

    // Reset all owned fields
    for (AABoardTile* Tile : OwnedTiles) {
        if (!Tile) continue;

        Tile->OwnerID = -1;
        Tile->UpgradeLevel = 0;
        Tile->bIsMortgaged = false;
    }

    OwnedTiles.Empty();
    OwnedTileCount = 0;

    // GAMEPLAY/ UI (ONLY IF WORLD EXISTS) 
    if (UWorld* World = GetWorld()) {
        if (AMyGameMode* GM = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(World))) {
            // Message 
            if (GM->CachedHubWidget) {
                FString Msg = FString::Printf(TEXT("Gracz %s zbankrutował i odpada z gry!"), *PlayerName);
                GM->CachedHubWidget->ShowMessage(Msg);
            }

            // Rank your position
            GM->AssignBankruptcyRank(this);

            // Character disappears after 2 seconds
            FTimerHandle DisappearHandle;
            World->GetTimerManager().SetTimer(DisappearHandle, [this]()
                {
                    SetActorHiddenInGame(true);
                    SetActorEnableCollision(false);
                    SetActorTickEnabled(false);
                }, 1.0f, false);

            // We skip the bankrupt's turn - switch to the next one
            FTimerHandle SkipHandle;
            World->GetTimerManager().SetTimer(SkipHandle, [GM]() { GM->NextTurn(); }, 1.f, false);

            // Check if there is only one active player left
            FTimerHandle CheckEndHandle;
            World->GetTimerManager().SetTimer(CheckEndHandle, [GM]() {
                int32 AliveCount = 0;

                // Count how many players are still playing
                for (APlayerCharacter* PC : GM->PlayerCharacters) {
                    if (PC && !PC->bIsBankrupt)
                        AliveCount++;
                }

                // If only one player remains
                if (AliveCount == 1) {
                    GM->CachedHubWidget->SetRollButtonVisible(false);

                    // Find the winner
                    APlayerCharacter* Winner = nullptr;
                    for (APlayerCharacter* PC : GM->PlayerCharacters) {
                        if (PC && !PC->bIsBankrupt) {
                            Winner = PC;
                            break;
                        }
                    }

                    // Display message on HUD
                    if (GM->CachedHubWidget && Winner) {
                        FString WinMsg = FString::Printf(TEXT("Gracz %s wygrał grę!"), *Winner->GetPlayerName());
                        GM->CachedHubWidget->ShowMessage(WinMsg);
                    }

                    // Display end screen after 3 seconds
                    GM->ShowFinalRankingWidget(3.0f);
                }
                }, 2.5f, false);
        }
    }
}

// Jail
void APlayerCharacter::SendToJail()
{
    AMyGameMode* GM = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
    if (!GM) return;

    // Find field 10
    for (AABoardTile* Tile : GM->BoardTiles) {
        if (Tile && Tile->TileID == 10) {
            SetActorLocation(Tile->GetActorLocation() + FVector(0, 0, 41.5f));
            CurrentTileIndex = Tile->TileID;
            TurnsInJail = 3;

            // Character rotation
            if (GetMesh()) {
                FRotator Rot = GetMesh()->GetComponentRotation();
                Rot.Yaw = 180.f;
                GetMesh()->SetWorldRotation(Rot);
            }

            // Camera Rotation (SpringArm)
            if (SpringArm) {
                FRotator ArmRot = SpringArm->GetRelativeRotation();
                ArmRot.Yaw = 90.f;
                SpringArm->SetRelativeRotation(ArmRot);
            }

            break;
        }
    }

    GM->bIsPlayerMoving = false;
    ConsecutiveDoubles = 0; // "Double" reset
    GM->bExtraRollGranted = false;
}

// Start
void APlayerCharacter::SendToStart()
{
    AMyGameMode* GM = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
    if (!GM) return;

    // find the START field (TileID == 0)
    for (AABoardTile* Tile : GM->BoardTiles) {
        if (Tile && Tile->TileID == 0) {
            // Transfer
            SetActorLocation(Tile->GetActorLocation() + FVector(0, 0, 41.5f));
            CurrentTileIndex = Tile->TileID;

            // Add a reward for passing START
            int32 Reward = Tile->TileCost; // it should be 200 ZR (or the value from BP)
            AddMoney(Reward);

            // Player rotation
            if (GetMesh()) {
                FRotator Rot = GetMesh()->GetComponentRotation();
                Rot.Yaw = 90.f;      // pionek patrzy "na planszę"
                GetMesh()->SetWorldRotation(Rot);
            }

            // Camera rotation 
            if (SpringArm) {
                FRotator ArmRot = SpringArm->GetRelativeRotation();
                ArmRot.Yaw = 0.f;
                SpringArm->SetRelativeRotation(ArmRot);
            }

            // HUD message
            if (GM->CachedHubWidget) {
                FString Msg = FString::Printf(TEXT("Przeniesiono cię na START. Otrzymujesz %d ZR."), Reward);
                GM->CachedHubWidget->ShowMessage(Msg);

                GM->CachedHubWidget->UpdateMoney(PlayerID, Money);
            }

            return;
        }
    }
}

// Library
void APlayerCharacter::SendToLibrary()
{
    AMyGameMode* GM = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
    if (!GM) return;

    for (AABoardTile* Tile : GM->BoardTiles){
        if (Tile && Tile->TileID == 28) {
            SetActorLocation(Tile->GetActorLocation() + FVector(0, 0, 41.5f));
            CurrentTileIndex = Tile->TileID;

            if (GetMesh()){
                FRotator Rot = GetMesh()->GetComponentRotation();
                Rot.Yaw = 270.f;
                GetMesh()->SetWorldRotation(Rot);
            }

            if (SpringArm){
                FRotator ArmRot = SpringArm->GetRelativeRotation();
                ArmRot.Yaw = 180.f;
                SpringArm->SetRelativeRotation(ArmRot);
            }

            if (GM->CachedHubWidget) GM->CachedHubWidget->ShowMessage(TEXT("Przeniesiono cię do biblioteki."));

            // czynsz jeśli ma właściciela
            if (Tile->OwnerID != -1 && Tile->OwnerID != PlayerID)
            {
                // Jesli pole zastawione
                if (Tile->bIsMortgaged){
                    if (GM->CachedHubWidget)
                        GM->CachedHubWidget->ShowMessage(TEXT("To pole jest zastawione – brak opłaty."));
                }else{
                    int Rent = Tile->GetUpgradedRent();

                    APlayerCharacter* OwnerPlayer = GM->GetPlayerByID(Tile->OwnerID);
                    if (OwnerPlayer){
                        // Remember before paying
                        int32 Before = Money;

                        RemoveMoney(Rent);

                        // if bankruptcy – pay back as much as you could
                        if (bIsBankrupt){
                            int32 ActuallyPaid = Before > 0 ? Before : 0;
                            OwnerPlayer->AddMoney(ActuallyPaid);

                            if (GM->CachedHubWidget){
                                FString Msg = FString::Printf(TEXT("Oddajesz %d ZR graczowi %s."), ActuallyPaid, *OwnerPlayer->GetPlayerName());

                                GM->CachedHubWidget->ShowMessage(Msg);
                                GM->CachedHubWidget->UpdateMoney(OwnerPlayer->PlayerID, OwnerPlayer->Money);
                            }
                            return;
                        }

                        OwnerPlayer->AddMoney(Rent);

                        if (GM->CachedHubWidget) {
                            GM->CachedHubWidget->UpdateMoney(PlayerID, Money);
                            GM->CachedHubWidget->UpdateMoney(OwnerPlayer->PlayerID, OwnerPlayer->Money);

                            FString Msg = FString::Printf( TEXT("Płacisz %d ZR graczowi %s za bibliotekę."), Rent, *OwnerPlayer->GetPlayerName());
                            GM->CachedHubWidget->ShowMessage(Msg);
                        }
                    }
                }
            }
            return;
        }
    }
}
