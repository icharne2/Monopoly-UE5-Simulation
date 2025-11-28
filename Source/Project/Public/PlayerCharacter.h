// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class PROJECT_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Sets default values for this character's properties
	APlayerCharacter();

	// Audio
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* OnLandSound; // dŸwiêk przy trafieniu do wiêzienia

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* StartTileSound; // dŸwiêk przy przejœciu przez START

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* TaxSound; // dŸwiêk podatku

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* ChanceCardSound; // dŸwiêk karty Szansy

	// Player camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* PlayerCamera;

	//Player Properties

	//Player ID (1-4)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Properties")
	int32 PlayerID = -1;

	//Player nick with default nick
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Properties")
	FString PlayerName = "Brak nazwy";

	// Set player name (to be called from BP)
	UFUNCTION(BlueprintCallable, Category = "Player Functions")
	void SetPlayerName(const FString& NewName);

	// Get player name 
	UFUNCTION(BlueprintPure, Category = "Player Functions")
	FString GetPlayerName() const;

	// BOARD POSITION
	// Current Tile Index
	UPROPERTY(BlueprintReadWrite, Category = "Board")
	int32 CurrentTileIndex = 0;

	// Distance between players on the same field
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Board")
	float PlayerOffsetDistance = 35.f;

	// Player movement
	UFUNCTION(BlueprintCallable, Category = "Board")
	void MoveBySteps(int32 Steps);

	// Economy/money
	//Amount of money - start amount is 1500
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Data")
	int32 Money = 1500;

	//Add money
	UFUNCTION(BlueprintCallable, Category = "Player Functions")
	void AddMoney(int32 Amount);

	//Subtract money
	UFUNCTION(BlueprintCallable, Category = "Player Functions")
	void RemoveMoney(int32 Amount);

	// Owned Tiles
	//Amount of owned tiles
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Data")
	int32 OwnedTileCount = 0;

	//List of owned tiles
	UPROPERTY(BlueprintReadOnly, Category = "Player")
	TArray<AABoardTile*> OwnedTiles;

	// Pledge and purchase of a field
	UFUNCTION(BlueprintCallable, Category = "Player Functions")
	void MortgageTile();

	UFUNCTION(BlueprintCallable, Category = "Player Functions")
	void RedeemTile();

	// Bonuses for special fields
	// Updating bonuses when having special fields
	UFUNCTION(BlueprintCallable, Category = "Bonuses")
	void UpdateRentBonus();

	// Rebuilds the list of owned fields from the board data (for fields assigned in BP)
	UFUNCTION(BlueprintCallable, Category = "Bonuses")
	void RebuildOwnedTilesFromBoard();

	// Rent bonuses for special fields
	UPROPERTY(BlueprintReadWrite, Category = "Bonuses")
	float RentBonusType1 = 1.0f;  // type 1: x3 fields
	UPROPERTY(BlueprintReadWrite, Category = "Bonuses")
	float RentBonusType2 = 1.0f;  // type 2: x4 fields

	// Jail system
	// Tours in prison
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 TurnsInJail = 0;

	// Tours from cards
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 TurnsToSkip = 0;

	// Jail pass
	UPROPERTY(BlueprintReadWrite, Category = "Player")
	bool bHasJailPass = false;

	// Closed laboratory operation
	UFUNCTION(BlueprintCallable, Category = "Player")
	bool IsInJail() const { return TurnsInJail > 0; }

	UFUNCTION() void SendToJail();
	UFUNCTION() void SendToStart();
	UFUNCTION() void SendToLibrary();

	// Add tile to owned list
	UFUNCTION(BlueprintCallable, Category = "Player Functions")
	void AddOwnedTile(AABoardTile* Tile);
			
	// Change animation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	UAnimationAsset* MoveAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	UAnimationAsset* IdleAnimation;

	void PlayMoveAnimation();
	void PlayIdleAnimation();
		
	// Throws
	// how many times in a row did he roll a double
	UPROPERTY(BlueprintReadWrite, Category = "Dice")
	int32 ConsecutiveDoubles = 0;

	// Bankrupt / Final score
	UPROPERTY(BlueprintReadOnly, Category = "Bankruptcy")
	bool bIsBankrupt = false;

	UPROPERTY(BlueprintReadOnly, Category = "Bankruptcy")
	int32 FinalRank = 0;

	UFUNCTION(BlueprintCallable, Category = "Bankruptcy")
	void DeclareBankruptcy();

	// Player's Final Score 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	int32 Score = 0;
		
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	// Help function that performs step-by-step movement
	void MoveToNextTile(class AMyGameMode* GM);

	// Number of steps remaining
	int32 RemainingSteps = 0;

	// Timer for smooth movement
	FTimerHandle MoveTimerHandle;

	// Duration of current move
	float ElapsedTime = 0.f;

};    

