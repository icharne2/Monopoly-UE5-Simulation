// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "WBP_FinalRankWidget.generated.h"

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
