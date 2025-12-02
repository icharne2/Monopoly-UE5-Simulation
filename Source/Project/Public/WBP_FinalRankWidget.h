#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "WBP_FinalRankWidget.generated.h"

/**
 * UWBP_FinalRankWidget
 * The "Game Over" / Leaderboard screen.
 * * It appears at the end of the game to show who won.
 * * It pulls data (Names and Scores) from the GameInstance and displays them.
 */

UCLASS()
class PROJECT_API UWBP_FinalRankWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
    // A function called when the widget appears on the screen (equivalent to BeginPlay)
    virtual void NativeConstruct() override;
public:
    // Rank text
    UPROPERTY(meta = (BindWidget)) UTextBlock* Rank1Text;
    UPROPERTY(meta = (BindWidget)) UTextBlock* Rank2Text;
    UPROPERTY(meta = (BindWidget)) UTextBlock* Rank3Text;
    UPROPERTY(meta = (BindWidget)) UTextBlock* Rank4Text;
    
    // Score text
    UPROPERTY(meta = (BindWidget)) UTextBlock* Score1Text;
    UPROPERTY(meta = (BindWidget)) UTextBlock* Score2Text;
    UPROPERTY(meta = (BindWidget)) UTextBlock* Score3Text;
    UPROPERTY(meta = (BindWidget)) UTextBlock* Score4Text;
	
};
