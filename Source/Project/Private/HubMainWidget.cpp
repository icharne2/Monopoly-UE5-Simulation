#include "HubMainWidget.h"
#include "MyGameInstance.h"
#include "MyGameMode.h"
#include "PlayerCharacter.h"
#include "ABoardTile.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CanvasPanel.h"

void UHubMainWidget::UpdateHUDFromGameInstance()
{
    UWorld* World = GetWorld();
    if (!World) return;

    UMyGameInstance* GameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(World));
    if (!GameInstance) return;

    if (NazwaGracza1)
        NazwaGracza1->SetText(FText::FromString(GameInstance->Player1Name));

    if (NazwaGracza2)
        NazwaGracza2->SetText(FText::FromString(GameInstance->Player2Name));

    if (NazwaGracza3)
        NazwaGracza3->SetText(FText::FromString(GameInstance->Player3Name));

    if (NazwaGracza4)
        NazwaGracza4->SetText(FText::FromString(GameInstance->Player4Name));
}

void UHubMainWidget::UpdateTurn(const FString& TurnText)
{
    if (Tura)
        Tura->SetText(FText::FromString(TurnText));
}

void UHubMainWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Rolling the dice
    if (Rzuc_kostka)
        Rzuc_kostka->OnClicked.AddDynamic(this, &UHubMainWidget::OnRollDiceClicked);

    // Buy a field
    if (Kup_pole) {
        Kup_pole->OnClicked.AddDynamic(this, &UHubMainWidget::OnBuyClicked);
        Kup_pole->SetVisibility(ESlateVisibility::Hidden); 
    }

    // Next turn
    if (NextTurnButton) {
        NextTurnButton->OnClicked.AddDynamic(this, &UHubMainWidget::OnNextTurnClicked);
        NextTurnButton->SetVisibility(ESlateVisibility::Hidden); 
    }

    // Pledge Field
    if (Zastaw_pole) {
        Zastaw_pole->OnClicked.AddDynamic(this, &UHubMainWidget::OnMortgageClicked);
        Zastaw_pole->SetVisibility(ESlateVisibility::Hidden);
    }

    // Redeem field
    if (Wykup_pole){
        Wykup_pole->OnClicked.AddDynamic(this, &UHubMainWidget::OnRedeemClicked);
        Wykup_pole->SetVisibility(ESlateVisibility::Hidden);
    }

    // FIELD CARD PREVIEW
    if (Podglad_karty){
        Podglad_karty->OnClicked.AddDynamic(this, &UHubMainWidget::OnPreviewCardClicked);
        Podglad_karty->SetVisibility(ESlateVisibility::Visible); 
    }

    // Close card
    if (Zamknij_karte)
        Zamknij_karte->OnClicked.AddDynamic(this, &UHubMainWidget::OnCloseCardClicked);

    // At the start, the field card panel is hidden
    if (KartaPola)
        KartaPola->SetVisibility(ESlateVisibility::Hidden);

    // Field improvement
    if (Ulepsz_pole){
        Ulepsz_pole->OnClicked.AddDynamic(this, &UHubMainWidget::OnUpgradeClicked);
        Ulepsz_pole->SetVisibility(ESlateVisibility::Hidden);
    }

    // Chance card
    if (zamknij)
        zamknij->OnClicked.AddDynamic(this, &UHubMainWidget::HideChanceCard);

    // Hide start panel
    if (Karta_szansy)
        Karta_szansy->SetVisibility(ESlateVisibility::Hidden);
}

void UHubMainWidget::OnRollDiceClicked()
{
    // CLOSE the field card panel if it is open
    if (KartaPola && KartaPola->GetVisibility() == ESlateVisibility::Visible)
        KartaPola->SetVisibility(ESlateVisibility::Hidden);

    UWorld* World = GetWorld();
    if (!World) return;

    AMyGameMode* GM = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(World));
    if (GM){
        GM->RollDice();
        SetRollButtonVisible(false);
    }
}

void UHubMainWidget::UpdateMoney(int32 PlayerID, int32 PlayerMoney)
{
    FString Text = FString::Printf(TEXT("%d ZR"), PlayerMoney);
    FText MoneyText = FText::FromString(Text);

    switch (PlayerID) {
        case 1:
            if (SaldoGracza1) SaldoGracza1->SetText(MoneyText);
            break;
        case 2:
            if (SaldoGracza2) SaldoGracza2->SetText(MoneyText);
            break;
        case 3:
            if (SaldoGracza3) SaldoGracza3->SetText(MoneyText);
            break;
        case 4:
            if (SaldoGracza4) SaldoGracza4->SetText(MoneyText);
            break;
        default:
            break;
    }
}

void UHubMainWidget::UpdateOwnedTiles(int32 PlayerID, int32 Count)
{
    FString Text = FString::Printf(TEXT("Liczba sal: %d"), Count);
    FText CountText = FText::FromString(Text);

    switch (PlayerID) {
        case 1:
            if (PolaGracza1) PolaGracza1->SetText(CountText);
            break;
        case 2:
            if (PolaGracza2) PolaGracza2->SetText(CountText);
            break;
        case 3:
            if (PolaGracza3) PolaGracza3->SetText(CountText);
            break;
        case 4:
            if (PolaGracza4) PolaGracza4->SetText(CountText);
            break;
        default:
            break;
    }
}

void UHubMainWidget::SetNextTurnButtonVisible(bool bVisible)
{
    if (NextTurnButton)
        NextTurnButton->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void UHubMainWidget::OnNextTurnClicked()
{
    // CLOSE the field card panel if it is open
    if (KartaPola && KartaPola->GetVisibility() == ESlateVisibility::Visible)
        KartaPola->SetVisibility(ESlateVisibility::Hidden);

    AMyGameMode* GM = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
    if (GM)
        GM->NextTurn();

    // Hide buttons after completing a turn
    SetNextTurnButtonVisible(false);
    SetBuyButtonVisible(false);
    HideChanceCard();

    // Hide pledge and buyback buttons after turn ends 
    SetMortgageButtonVisible(false);
    SetRedeemButtonVisible(false);
    
    UE_LOG(LogTemp, Warning, TEXT("[HUD] Next turn clicked - hiding the Pledge/Buy buttons"));

    // Hide upgrade field
    SetUpgradeButtonVisible(false);
}

void UHubMainWidget::SetBuyButtonVisible(bool bVisible)
{
    if (Kup_pole)
        Kup_pole->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void UHubMainWidget::OnBuyClicked()
{
    UWorld* World = GetWorld();
    if (!World) return;

    AMyGameMode* GM = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(World));
    if (!GM) return;

    APlayerCharacter* CurrentPlayer = GM->GetPlayerByID(GM->CurrentPlayerID);
    if (!CurrentPlayer) return;

    // Get the current field
    if (GM->BoardTiles.Num() == 0) return;
    AABoardTile* CurrentTile = GM->BoardTiles[CurrentPlayer->CurrentTileIndex];
    if (!CurrentTile) return;

    // Check if the field can be purchased
    if (CurrentTile->bCanBeBought && CurrentTile->OwnerID == -1)
    {
        int32 Price = CurrentTile->TileCost;

        if (CurrentPlayer->Money >= Price)
        {
            if (BuyTileSound)
                UGameplayStatics::PlaySound2D(this, BuyTileSound, 0.8f);

            // Buying a field
            CurrentPlayer->RemoveMoney(Price);
            CurrentTile->OwnerID = CurrentPlayer->PlayerID;
            CurrentPlayer->AddOwnedTile(CurrentTile);

            // HUD update
            if (GM->CachedHubWidget){
                GM->CachedHubWidget->UpdateMoney(CurrentPlayer->PlayerID, CurrentPlayer->Money);
                GM->CachedHubWidget->UpdateOwnedTiles(CurrentPlayer->PlayerID, CurrentPlayer->OwnedTileCount);
            }

            // Hide button after purchase
            SetBuyButtonVisible(false);

            FString Msg = FString::Printf(TEXT("Masz nowe pole za %d ZR!"), Price);
            ShowMessage(Msg);
        } else {
            if (GM && GM->CachedHubWidget) {
                FString Msg = FString::Printf(TEXT("Nie mozesz kupic tego pola! (%s)"), *CurrentTile->TileName);
                ShowMessage(Msg);
            }
        }
    }

    if (KartaPola)
        KartaPola->SetVisibility(ESlateVisibility::Hidden);
}

void UHubMainWidget::SetRollButtonVisible(bool bVisible)
{
    if (Rzuc_kostka)
        Rzuc_kostka->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void UHubMainWidget::ShowDiceResult(int32 DiceA, int32 DiceB)
{
    if (!WylosowanoText)
        return;

    FString Text = FString::Printf(TEXT("Wylosowano: %d oraz %d"), DiceA, DiceB);
    WylosowanoText->SetText(FText::FromString(Text));
    WylosowanoText->SetVisibility(ESlateVisibility::Visible);

    // Hide text after 2 seconds
    FTimerHandle HideTimer;
    GetWorld()->GetTimerManager().SetTimer(HideTimer, [this]()
        {
            if (WylosowanoText){
                WylosowanoText->SetVisibility(ESlateVisibility::Hidden);
            }
        }, 2.f, false);
}

void UHubMainWidget::ShowMessage(const FString& NewMessage, float Duration)
{
    if (!Message) return;

    Message->SetText(FText::FromString(NewMessage));
    Message->SetVisibility(ESlateVisibility::Visible);

    // Hide after time (Duration)
    FTimerHandle HideHandle;
    GetWorld()->GetTimerManager().SetTimer(HideHandle, [this]()
        {
            if (Message){
                Message->SetVisibility(ESlateVisibility::Hidden);
            }
        }, Duration, false);
}

void UHubMainWidget::SetMortgageButtonVisible(bool bVisible)
{
    if (Zastaw_pole)
        Zastaw_pole->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void UHubMainWidget::SetRedeemButtonVisible(bool bVisible)
{
    if (Wykup_pole)
        Wykup_pole->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void UHubMainWidget::OnMortgageClicked()
{
    AMyGameMode* GM = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
    if (!GM) return;

    APlayerCharacter* CurrentPlayer = GM->GetPlayerByID(GM->CurrentPlayerID);
    if (CurrentPlayer) {
        CurrentPlayer->MortgageTile();
        SetMortgageButtonVisible(false);
    }
}

void UHubMainWidget::OnRedeemClicked()
{
    AMyGameMode* GM = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
    if (!GM) return;

    APlayerCharacter* CurrentPlayer = GM->GetPlayerByID(GM->CurrentPlayerID);
    if (CurrentPlayer) {
        CurrentPlayer->RedeemTile();
        SetRedeemButtonVisible(false);
    }
}

void UHubMainWidget::UpdateTileCard(AABoardTile* Tile)
{
    if (!Tile) return;
    CurrentTileRef = Tile;

    int32 ID = Tile->TileID;

    // Hide everything at start
    if (Cena1Ulepszenia) Cena1Ulepszenia->SetText(FText::GetEmpty());
    if (Cena2Ulepszenia) Cena2Ulepszenia->SetText(FText::GetEmpty());
    if (Cena3Ulepszenia) Cena3Ulepszenia->SetText(FText::GetEmpty());
    if (Podatek) Podatek->SetText(FText::GetEmpty());
    if (cena_zakupu) cena_zakupu->SetText(FText::GetEmpty());
    if (po_ulepszeniu1) po_ulepszeniu1->SetText(FText::GetEmpty());
    if (po_ulepszeniu2) po_ulepszeniu2->SetText(FText::GetEmpty());
    if (po_ulepszeniu3) po_ulepszeniu3->SetText(FText::GetEmpty());
    if (wlasciciel) wlasciciel->SetText(FText::GetEmpty());
    if (zastawienie_info) zastawienie_info->SetVisibility(ESlateVisibility::Hidden);
    if(wykupienie_info) wykupienie_info->SetVisibility(ESlateVisibility::Hidden);

    // Start field
    if (ID == 0) {
        if (nazwa_pola) {
            FString NazwaTekst = FString::Printf(TEXT("Nazwa pola: %s."), *Tile->TileName);
            nazwa_pola->SetText(FText::FromString(NazwaTekst));
        }

        if (cena_zakupu)
            cena_zakupu->SetText(FText::FromString(TEXT("Otrzymujesz 200 ZR jako stypendium motywacyjne za sam fakt, że nadal się uczysz. Uczelnia jest z Ciebie dumna… choć jeszcze o tym nie wie.")));

        return;
    }

    // Fields od chance
    if (ID == 8 || ID == 23 || ID == 32) {
        if (nazwa_pola) {
            FString NazwaTekst = FString::Printf(TEXT("Nazwa pola: %s."), *Tile->TileName);
            nazwa_pola->SetText(FText::FromString(NazwaTekst));
        }

        if (cena_zakupu)
            cena_zakupu->SetText(FText::FromString(TEXT("Czas na Szansę! Może dziekan odwołał kolokwium, a może system USOS znów wyzerował Twoje punkty. Los bywa kapryśny - weź karte i przekonaj się!")));

        return;
    }

    // Jail 
    if (ID == 10) {
        if (nazwa_pola) {
            FString NazwaTekst = FString::Printf(TEXT("Nazwa pola: %s."), *Tile->TileName);
            nazwa_pola->SetText(FText::FromString(NazwaTekst));
        }

        if (cena_zakupu)
            cena_zakupu->SetText(FText::FromString(TEXT("Laboratorium zostało zamknięte po serii dziwnych zdarzeń. Świadkowie twierdzą, że trzy myszy w fartuchach prowadzą badania, które po raz pierwszy dają powtarzalne wyniki.")));

        return;
    }

    // Extreme tax
    if (ID == 17 || ID == 38)
    {
        if (nazwa_pola) {
            FString NazwaTekst = FString::Printf(TEXT("Nazwa pola: %s."), *Tile->TileName);
            nazwa_pola->SetText(FText::FromString(NazwaTekst));
        }

        if (cena_zakupu)
            cena_zakupu->SetText(FText::FromString(TEXT("Trafiasz na Podatek Ekstremalny! Dziekanat przypomina Ci o wszystkich nieopłaconych składkach od pierwszego roku. 200 ZR znika szybciej niż motywacja po zdanej sesji.")));

        return;
    }

    // Aquarium - resting field
    if (ID == 20 ) {
        if (nazwa_pola) {
            FString NazwaTekst = FString::Printf(TEXT("Nazwa pola: %s"), *Tile->TileName);
            nazwa_pola->SetText(FText::FromString(NazwaTekst));
        }

        if (cena_zakupu)
            cena_zakupu->SetText(FText::FromString(TEXT("Akwarium - oaza spokoju pośród akademickiego chaosu. Tutaj studenci odłączają się od rzeczywistości, ładują baterie i udają, że notatki same się nauczą.")));

        return;
    }

    // Student tax
    if (ID == 6) {
        if (nazwa_pola) {
            FString NazwaTekst = FString::Printf(TEXT("Nazwa pola: %s."), *Tile->TileName);
            nazwa_pola->SetText(FText::FromString(NazwaTekst));
        }

        if (cena_zakupu)
            cena_zakupu->SetText(FText::FromString(TEXT("Uczelnia przypomina sobie o Twoich zaległościach. Płacisz 100 ZR tytułem „dobrowolnej” opłaty za coś, czego nie pamiętasz. Może to legitymacja… a może kawa w dziekanacie ? ")));

        return;
    }

    // Type 2 Fields (no upgrades)
    if ( ID == 5 || ID == 15 || ID == 25 || ID == 35)
    {
        const int32 BaseRent1 = Tile->GetBaseRent(); // bazowo 20% ceny
        const int32 Rent2 = FMath::RoundToInt(BaseRent1 * 1.05f);  // 2 pola
        const int32 Rent3 = FMath::RoundToInt(BaseRent1 * 1.12f);  // 3 pola
        const int32 Rent4 = FMath::RoundToInt(BaseRent1 * 1.20f);  // 4 pola

        if (nazwa_pola) {
            FString NazwaTekst = FString::Printf(TEXT("Nazwa pola: %s."), *Tile->TileName);
            nazwa_pola->SetText(FText::FromString(NazwaTekst));
        }

        if (cena_zakupu) {
            FString CenaTekst = FString::Printf(TEXT("Cena zakupu: %d ZR."), Tile->TileCost);
            cena_zakupu->SetText(FText::FromString(CenaTekst));
        }

        if (Podatek) {
            FString PodatekTekst = FString::Printf(TEXT("Podatek podstawowy: %d ZR."), Tile->GetBaseRent());
            Podatek->SetText(FText::FromString(PodatekTekst));
        }

        if (po_ulepszeniu1) {
            FString Ulep1 = FString::Printf(TEXT("Podatek, gdy 2 pola: %d ZR."), Rent2);
            po_ulepszeniu1->SetText(FText::FromString(Ulep1));
        }

        if (po_ulepszeniu2) {
            FString Ulep1 = FString::Printf(TEXT("Podatek, gdy 3 pola: %d ZR."), Rent3);
            po_ulepszeniu2->SetText(FText::FromString(Ulep1));
        }

        if (po_ulepszeniu3) {
            FString Ulep1 = FString::Printf(TEXT("Podatek, gdy 4 pola: %d ZR."), Rent4);
            po_ulepszeniu3->SetText(FText::FromString(Ulep1));
        }

        if (zastawienie_info)
            zastawienie_info->SetVisibility(ESlateVisibility::Visible);

        if (wykupienie_info)
            wykupienie_info->SetVisibility(ESlateVisibility::Visible);

        UpdateOwnerText(Tile);
        return;
    }

    // Type 1 Fields (no upgrades)
    if (ID == 3 || ID == 12 || ID == 28)
    {
        const int32 BaseRent2 = Tile->GetBaseRent(); // bazowo 20% ceny
        const int32 Rent = FMath::RoundToInt(BaseRent2 * 1.10f);  // wszytkiep pola

        if (nazwa_pola) {
            FString NazwaTekst = FString::Printf(TEXT("Nazwa pola: %s."), *Tile->TileName);
            nazwa_pola->SetText(FText::FromString(NazwaTekst));
        }

        if (cena_zakupu) {
            FString CenaTekst = FString::Printf(TEXT("Cena zakupu: %d ZR"), Tile->TileCost);
            cena_zakupu->SetText(FText::FromString(CenaTekst));
        }

        if (Podatek) {
            FString PodatekTekst = FString::Printf(TEXT("Podatek podstawowy: %d ZR"), Tile->GetBaseRent());
            Podatek->SetText(FText::FromString(PodatekTekst));
        }

        if (po_ulepszeniu1) {
            FString Ulep1 = FString::Printf(TEXT("Podatek, gdy gracz posiada 3 pola specialne typu 1: %d ZR"), Rent);
            po_ulepszeniu1->SetText(FText::FromString(Ulep1));
        }

        if (zastawienie_info)
            zastawienie_info->SetVisibility(ESlateVisibility::Visible);

        if (wykupienie_info)
            wykupienie_info->SetVisibility(ESlateVisibility::Visible);

        UpdateOwnerText(Tile);
        return;
    }

    // Normal fields (with upgrades)
    if (nazwa_pola) {
        FString NazwaTekst = FString::Printf(TEXT("Nazwa pola: %s"), *Tile->TileName);
        nazwa_pola->SetText(FText::FromString(NazwaTekst));
    }

    if (cena_zakupu) {
        FString CenaZakupuTekst = FString::Printf(TEXT("Cena zakupu: %d ZR"), Tile->TileCost);
        cena_zakupu->SetText(FText::FromString(CenaZakupuTekst));
    }

    if (Cena1Ulepszenia) {
        FString Ulep1 = FString::Printf(TEXT("Cena 1 ulepszenia: %d ZR"), Tile->UpgradeCostLevel1);
        Cena1Ulepszenia->SetText(FText::FromString(Ulep1));
    }

    if (Cena2Ulepszenia) {
        FString Ulep2 = FString::Printf(TEXT("Cena 2 ulepszenia: %d ZR"), Tile->UpgradeCostLevel2);
        Cena2Ulepszenia->SetText(FText::FromString(Ulep2));
    }

    if (Cena3Ulepszenia) {
        FString Ulep3 = FString::Printf(TEXT("Cena 3 ulepszenia: %d ZR"), Tile->UpgradeCostLevel3);
        Cena3Ulepszenia->SetText(FText::FromString(Ulep3));
    }

    if (Podatek) {
        FString PodatekTekst = FString::Printf(TEXT("Podatek: %d ZR"), Tile->GetBaseRent());
        Podatek->SetText(FText::FromString(PodatekTekst));
    }

    // Tax after upgrade
    if (po_ulepszeniu1) {
        int32 Rent1 = Tile->GetRentForLevel(1);
        FString Text = FString::Printf(TEXT("Po 1 ulepszeniu: %d ZR"), Rent1);
        po_ulepszeniu1->SetText(FText::FromString(Text));
    }

    if (po_ulepszeniu2) {
        int32 Rent2 = Tile->GetRentForLevel(2);
        FString Text = FString::Printf(TEXT("Po 2 ulepszeniu: %d ZR"), Rent2);
        po_ulepszeniu2->SetText(FText::FromString(Text));
    }

    if (po_ulepszeniu3) {
        int32 Rent3 = Tile->GetRentForLevel(3);
        FString Text = FString::Printf(TEXT("Po 3 ulepszeniu: %d ZR"),Rent3);
        po_ulepszeniu3->SetText(FText::FromString(Text));
    }

    if(zastawienie_info)
        zastawienie_info->SetVisibility(ESlateVisibility::Visible);

    if(wykupienie_info)
        wykupienie_info->SetVisibility(ESlateVisibility::Visible);

    UpdateOwnerText(Tile);

    // Button always visible
    if (Podglad_karty)
        Podglad_karty->SetVisibility(ESlateVisibility::Visible);
}

void UHubMainWidget::UpdateOwnerText(AABoardTile* Tile)
{
    if (!Tile || !wlasciciel) return;

    FString OwnerText;

    if (Tile->OwnerID == -1)
    {
        OwnerText = TEXT("Brak");
    } else {
        // Get player name from GameInstance
        UMyGameInstance* GI = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
        if (GI) {
            switch (Tile->OwnerID) {
                case 1: OwnerText = GI->Player1Name; break;
                case 2: OwnerText = GI->Player2Name; break;
                case 3: OwnerText = GI->Player3Name; break;
                case 4: OwnerText = GI->Player4Name; break;
                default: OwnerText = TEXT("Nieznany"); break;
            }
        } else {
            OwnerText = FString::Printf(TEXT("Gracz %d"), Tile->OwnerID);
        }
    }

    FString FullText = FString::Printf(TEXT("Właściciel: %s"), *OwnerText);
    wlasciciel->SetText(FText::FromString(FullText));
}

// Open/Close field tab
void UHubMainWidget::OnPreviewCardClicked()
{
    if (!KartaPola)
        return;

    // Get player name
    UWorld* World = GetWorld();
    if (!World) return;

    AMyGameMode* GM = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(World));
    if (!GM) return;

    APlayerCharacter* CurrentPlayer = GM->GetPlayerByID(GM->CurrentPlayerID);
    if (CurrentPlayer && GM->BoardTiles.IsValidIndex(CurrentPlayer->CurrentTileIndex))
    {
        AABoardTile* Tile = GM->BoardTiles[CurrentPlayer->CurrentTileIndex];
        UpdateTileCard(Tile); // refresh card details

        if (OpenTileCardSound)
            UGameplayStatics::PlaySound2D(this, OpenTileCardSound);
    }

    // show/hide panel
    bool bIsVisible = (KartaPola->GetVisibility() == ESlateVisibility::Visible);
    KartaPola->SetVisibility(bIsVisible ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
}

void UHubMainWidget::OnCloseCardClicked()
{
    if (KartaPola)
        KartaPola->SetVisibility(ESlateVisibility::Hidden);
}

void UHubMainWidget::SetPreviewButtonVisible(bool bVisible)
{
    if (Podglad_karty)
        Podglad_karty->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

// Field improvement
void UHubMainWidget::SetUpgradeButtonVisible(bool bVisible)
{
    UE_LOG(LogTemp, Warning, TEXT("[HUD][DEBUG] SetUpgradeButtonVisible(%s) called"),  bVisible ? TEXT("true") : TEXT("false"));

    if (Ulepsz_pole) {
        UE_LOG(LogTemp, Warning, TEXT("[HUD][DEBUG] Current visibility  Ulepsz_pole BEFORE: %s"), *UEnum::GetValueAsString(Ulepsz_pole->GetVisibility()));
        Ulepsz_pole->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
    }
}

void UHubMainWidget::OnUpgradeClicked()
{
    AMyGameMode* GM = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
    if (!GM) return;

    APlayerCharacter* CurrentPlayer = GM->GetPlayerByID(GM->CurrentPlayerID);
    if (!CurrentPlayer) return;

    if (GM->BoardTiles.Num() == 0) return;

    AABoardTile* CurrentTile = GM->BoardTiles[CurrentPlayer->CurrentTileIndex];
    if (!CurrentTile || CurrentTile->OwnerID != CurrentPlayer->PlayerID)
    {
        ShowMessage(TEXT("Nie możesz ulepszyć tego pola!"), 2.f);
        return;
    }

    // Check if the player has all the fields in the color group
    FString Group = CurrentTile->ColorGroup;
    bool bHasAllInGroup = true;

    for (AABoardTile* Tile : GM->BoardTiles) {
        if (Tile && Tile->ColorGroup == Group && Tile->OwnerID != CurrentPlayer->PlayerID) {
            bHasAllInGroup = false;
            break;
        }
    }

    if (!bHasAllInGroup) {
        ShowMessage(TEXT("Nie posiadasz wszystkich pól z tej grupy kolorystycznej!"), 2.f);
        return;
    }

    // Check if the field is not blocked
    if (CurrentTile->bIsMortgaged) {
        ShowMessage(TEXT("Pole jest zastawione — nie można go ulepszyć!"), 2.f);
        return;
    }

    // Check the upgrade level
    if (CurrentTile->UpgradeLevel >= 3) {
        ShowMessage(TEXT("Pole jest już maksymalnie ulepszone!"), 2.f);
        return;
    }

    // Determine the cost of the current level
    int32 UpgradeCost = 0;
    switch (CurrentTile->UpgradeLevel + 1) {
        case 1: UpgradeCost = CurrentTile->UpgradeCostLevel1; break;
        case 2: UpgradeCost = CurrentTile->UpgradeCostLevel2; break;
        case 3: UpgradeCost = CurrentTile->UpgradeCostLevel3; break;
    }

    // Does the player have enough money?
    if (CurrentPlayer->Money < UpgradeCost) {
        ShowMessage(TEXT("Nie masz wystarczająco Złotych Rud!"), 2.f);
        return;
    }

    // Collect the fee and upgrade the field
    CurrentPlayer->RemoveMoney(UpgradeCost);
    bool bUpgraded = CurrentTile->UpgradeTile();

    if (bUpgraded) {
        FString Msg = FString::Printf(TEXT("Ulepszyłeś pole: %s (poziom %d)"), *CurrentTile->TileName, CurrentTile->UpgradeLevel);
        ShowMessage(Msg, 2.f);

        if (GM->CachedHubWidget)
            GM->CachedHubWidget->UpdateMoney(CurrentPlayer->PlayerID, CurrentPlayer->Money);
    } else {
        ShowMessage(TEXT("Nie udało się ulepszyć pola!"), 2.f);
    }

    // Hide button after upgrade
    SetUpgradeButtonVisible(false);
}

// Chance cards
void UHubMainWidget::ShowChanceCard(const FString& Text)
{
    if (Opis) 
        Opis->SetText(FText::FromString(Text));

    if (Karta_szansy)
        Karta_szansy->SetVisibility(ESlateVisibility::Visible);

    // Play the opening sound
    if (OpenTileCardSound)
        UGameplayStatics::PlaySound2D(this, OpenTileCardSound);
}

void UHubMainWidget::HideChanceCard()
{
    if (Karta_szansy)
        Karta_szansy->SetVisibility(ESlateVisibility::Hidden);

    // GameMode will execute the effect only after HideChanceCard()
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0)) {
        if (AMyGameMode* GM = Cast<AMyGameMode>(PC->GetWorld()->GetAuthGameMode())) 
            GM->ApplyPendingChanceEffect();
    }
}
