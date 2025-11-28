// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyGameMode.generated.h"

class ADiceActor;
class APlayerCharacter;
class UHubMainWidget;
class ACameraActor;
class UWBP_FinalRankWidget;

// Chance Card
USTRUCT(BlueprintType)
struct FChanceCard
{
	GENERATED_BODY()

public:

	UPROPERTY()
	FString Description;

	UPROPERTY()
	int32 EffectID = 0;

	FChanceCard() {}
	FChanceCard(const FString& Desc, int32 ID)
		: Description(Desc), EffectID(ID) {
	}
};


// Game mode class
UCLASS()
class PROJECT_API AMyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
    AMyGameMode();

    virtual void BeginPlay() override;

	// Chance cards
	UPROPERTY()
	TArray<FChanceCard> ChanceCards;

	UPROPERTY()
	int32 PendingChanceEffect = -1;

	UPROPERTY()
	int32 PendingChancePlayerID = -1;   

	// Drawing a chance card
	UFUNCTION()
	void TriggerChanceCard(int32 PlayerID);

	UFUNCTION()
	void ApplyPendingChanceEffect();

	// Dice system
	// Music (the sound of rolling the dice)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* DiceRollSound;

	// Phisical dice
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dice")
	ADiceActor* Dice1Ref;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dice")
	ADiceActor* Dice2Ref;

	// Camera for dice
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	ACameraActor* DiceCamera;

	UFUNCTION(BlueprintCallable, Category = "Camera")
	void FocusCameraOnDice();

	// Internal roll state
	int32 DiceStoppedCount = 0;
	bool bIsRolling = false;

	// Dice results 
	int32 Dice1Value = 0;
	int32 Dice2Value = 0;

	// Starting positions for the dice
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dice")
	FVector Dice1StartLocation = FVector(-60, 0, 30);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dice")
	FVector Dice2StartLocation = FVector(10, 10, 30);

	// Callback when the dices stops
	UFUNCTION()
	void OnDiceStoppedHandler();

	void HandleDiceResult(int32 Dice1, int32 Dice2);

	// Turn system
	UPROPERTY(BlueprintReadOnly, Category = "Turn")
	int32 CurrentPlayerID = 1; // which player has a turn (1-4)

	// List of all players
	UPROPERTY(BlueprintReadOnly, Category = "Turn")
	TArray<class APlayerCharacter*> PlayerCharacters;

	// Game functions
	UFUNCTION(BlueprintCallable, Category = "Turn")
	void StartGameSetup(); // assign players, names, set start

	UFUNCTION(BlueprintCallable, Category = "Turn")
	void NextTurn(); // switches turn

	UFUNCTION(BlueprintCallable, Category = "Turn")
	void RollDice(); // dice roll (log + turn change)

	// Whether the player's piece is currently moving (dice click lock)
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Turn")
	bool bIsPlayerMoving = false;

	// Whether the player has one more throw after the double
	UPROPERTY(BlueprintReadWrite, Category = "Dice")
	bool bExtraRollGranted = false;
	// Camera
	UFUNCTION(BlueprintCallable, Category = "Camera")
	void FocusCameraOnCurrentPlayer();

	// Setting the path on the board
	// List of board squares
	UPROPERTY(BlueprintReadOnly, Category = "Board")
	TArray<class AABoardTile*> BoardTiles;

	// Find a player by ID
	UFUNCTION(BlueprintCallable, Category = "Players")
	class APlayerCharacter* GetPlayerByID(int32 PlayerID);
		
	// Bankrupcity
	int32 BankruptcyCounter = 0;
	void AssignBankruptcyRank(APlayerCharacter* BankruptPlayer);

	// HUD reference (to update subtitles)
	UPROPERTY(BlueprintReadOnly, Category = "UI")
	class UHubMainWidget* CachedHubWidget;

	// Final wbp
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> FinalRankWidgetClass;

	UFUNCTION(BlueprintCallable)
	void ShowFinalRankingWidget(float Delay=3.5);

};
