#include "WBP_FinalRankWidget.h"
#include "MyGameInstance.h"
#include "Kismet/GameplayStatics.h"

void UWBP_FinalRankWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Remove HUD
    if (UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance())) {
        auto SetSafe = [](UTextBlock* Txt, const FString& Val) {
                if (Txt)
                    Txt->SetText(FText::FromString(Val));
            };

        // Load data from GameInstance
        if (GI->FinalPlayerNames.Num() > 0) {
            if (Rank1Text) SetSafe(Rank1Text, GI->FinalPlayerNames[0]);
            if (Score1Text && GI->FinalScores.Num() > 0)
                SetSafe(Score1Text, FString::FromInt(GI->FinalScores[0]));
        }

        if (GI->FinalPlayerNames.Num() > 1) {
            if (Rank2Text) SetSafe(Rank2Text, GI->FinalPlayerNames[1]);
            if (Score2Text && GI->FinalScores.Num() > 1)
                SetSafe(Score2Text, FString::FromInt(GI->FinalScores[1]));
        }

        if (GI->FinalPlayerNames.Num() > 2) {
            if (Rank3Text) SetSafe(Rank3Text, GI->FinalPlayerNames[2]);
            if (Score3Text && GI->FinalScores.Num() > 2)
                SetSafe(Score3Text, FString::FromInt(GI->FinalScores[2]));
        }

        if (GI->FinalPlayerNames.Num() > 3) {
            if (Rank4Text) SetSafe(Rank4Text, GI->FinalPlayerNames[3]);
            if (Score4Text && GI->FinalScores.Num() > 3)
                SetSafe(Score4Text, FString::FromInt(GI->FinalScores[3]));
        }
        UE_LOG(LogTemp, Warning, TEXT("FinalRankWidget: data loaded from GameInstance."));

    }
}
