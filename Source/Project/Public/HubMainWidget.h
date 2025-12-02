#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HubMainWidget.generated.h"

class UTextBlock;
class UButton;
class UCanvasPanel;
class AABoardTile;

/**
 * UHubMainWidget
 * The main HUD (Heads-Up Display) for the game.
 * It shows player stats (money, owned tiles), turn info, dice results,
 * and handles all UI interactions like buttons for buying, rolling dice, etc.
 */

UCLASS()
class PROJECT_API UHubMainWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    //Update functions

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateHUDFromGameInstance();

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateTurn(const FString& TurnText);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateMoney(int32 PlayerID, int32 PlayerMoney);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateOwnedTiles(int32 PlayerID, int32 Count);

    // Button visibility controls

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void SetBuyButtonVisible(bool bVisible);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void SetNextTurnButtonVisible(bool bVisible);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void SetRollButtonVisible(bool bVisible);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void SetMortgageButtonVisible(bool bVisible);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void SetRedeemButtonVisible(bool bVisible);

    // Alerts and info

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void ShowDiceResult(int32 DiceA, int32 DiceB);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void ShowMessage(const FString& NewMessage, float Duration = 2.0f);

    // Tiles card

    // Field card data update
    UFUNCTION(BlueprintCallable, Category = "HUD|Tile")
    void UpdateTileCard(AABoardTile* Tile);

    // Sets the visibility of the "Card Preview" button
    UFUNCTION(BlueprintCallable, Category = "HUD|Tile")
    void SetPreviewButtonVisible(bool bVisible);

    // Sets the visibility of the upgrade button
    UFUNCTION(BlueprintCallable, Category = "UI")
    void SetUpgradeButtonVisible(bool bVisible);

    // Music
    // Sound when purchasing a box
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    USoundBase* BuyTileSound;

    // Sound when opening a field card and for a chance
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    USoundBase* OpenTileCardSound;

    UFUNCTION() void ShowChanceCard(const FString& Text);
    UFUNCTION() void HideChanceCard();


protected:
    virtual void NativeConstruct() override;

    // Click Handling Functions (Internal)
    UFUNCTION() void OnRollDiceClicked();
    UFUNCTION() void OnBuyClicked();
    UFUNCTION() void OnNextTurnClicked();
    UFUNCTION() void OnMortgageClicked();
    UFUNCTION() void OnRedeemClicked();
    UFUNCTION() void OnPreviewCardClicked();
    UFUNCTION() void OnCloseCardClicked();
    UFUNCTION() void OnUpgradeClicked();

private:

    // Buttons and text fields for the odds card
    // Chance card
    UPROPERTY(meta = (BindWidgetOptional)) UCanvasPanel* Karta_szansy;
    UPROPERTY(meta = (BindWidgetOptional)) UTextBlock* Opis;
    UPROPERTY(meta = (BindWidgetOptional)) UButton* zamknij;

    // Player buttons and text boxes
    UPROPERTY(meta = (BindWidget)) class UTextBlock* NazwaGracza1;
    UPROPERTY(meta = (BindWidget)) class UTextBlock* NazwaGracza2;
    UPROPERTY(meta = (BindWidget)) class UTextBlock* NazwaGracza3;
    UPROPERTY(meta = (BindWidget)) class UTextBlock* NazwaGracza4;

    UPROPERTY(meta = (BindWidget)) class UTextBlock* SaldoGracza1;
    UPROPERTY(meta = (BindWidget)) class UTextBlock* SaldoGracza2;
    UPROPERTY(meta = (BindWidget)) class UTextBlock* SaldoGracza3;
    UPROPERTY(meta = (BindWidget)) class UTextBlock* SaldoGracza4;

    UPROPERTY(meta = (BindWidget)) class UTextBlock* PolaGracza1;
    UPROPERTY(meta = (BindWidget)) class UTextBlock* PolaGracza2;
    UPROPERTY(meta = (BindWidget)) class UTextBlock* PolaGracza3;
    UPROPERTY(meta = (BindWidget)) class UTextBlock* PolaGracza4;

    UPROPERTY(meta = (BindWidget)) class UTextBlock* WylosowanoText;
    UPROPERTY(meta = (BindWidget)) class UTextBlock* Tura;
    UPROPERTY(meta = (BindWidget)) class UTextBlock* Message;
    
    // Action button
    UPROPERTY(meta = (BindWidget)) class UButton* Kup_pole;
    UPROPERTY(meta = (BindWidget)) class UButton* NextTurnButton;
    UPROPERTY(meta = (BindWidget)) class UButton* Rzuc_kostka;
    UPROPERTY(meta = (BindWidget)) class UButton* Zastaw_pole;
    UPROPERTY(meta = (BindWidget)) class UButton* Wykup_pole;
    UPROPERTY(meta = (BindWidget)) class UButton* Ulepsz_pole;

    // Field tab and preview buttons
    UPROPERTY(meta = (BindWidgetOptional)) UCanvasPanel* KartaPola;
    UPROPERTY(meta = (BindWidgetOptional)) UButton* Podglad_karty;
    UPROPERTY(meta = (BindWidgetOptional)) UButton* Zamknij_karte;

    // Field card texts
    UPROPERTY(meta = (BindWidgetOptional)) UTextBlock* nazwa_pola;
    UPROPERTY(meta = (BindWidgetOptional)) UTextBlock* cena_zakupu;
    UPROPERTY(meta = (BindWidgetOptional)) UTextBlock* Cena1Ulepszenia;
    UPROPERTY(meta = (BindWidgetOptional)) UTextBlock* Cena2Ulepszenia;
    UPROPERTY(meta = (BindWidgetOptional)) UTextBlock* Cena3Ulepszenia;
    UPROPERTY(meta = (BindWidgetOptional)) UTextBlock* Podatek;
    UPROPERTY(meta = (BindWidgetOptional)) UTextBlock* po_ulepszeniu1;
    UPROPERTY(meta = (BindWidgetOptional)) UTextBlock* po_ulepszeniu2;
    UPROPERTY(meta = (BindWidgetOptional)) UTextBlock* po_ulepszeniu3;
    UPROPERTY(meta = (BindWidgetOptional)) UTextBlock* wlasciciel;
    UPROPERTY(meta = (BindWidgetOptional)) UTextBlock* zastawienie_info;
    UPROPERTY(meta = (BindWidgetOptional)) UTextBlock* wykupienie_info;

    // Reference to the current field
    UPROPERTY() AABoardTile* CurrentTileRef;  
    void UpdateOwnerText(AABoardTile* Tile);
};
