// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ABoardTile.generated.h"

UCLASS()
class PROJECT_API AABoardTile : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties - constructor
	AABoardTile();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called when the actor is constructed or modified (in editor or during spawn)
	virtual void OnConstruction(const FTransform& Transform) override;

	// TILE PROPERTIES
	// Cost of the cube
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Properties")
	int32 TileCost = 100;

	// Cost of upgrade level 1
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Properties")
	int32 UpgradeCostLevel1 = 50;

	// Cost of upgrade level 2
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Properties")
	int32 UpgradeCostLevel2 = 100;

	// Cost of upgrade level 3
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Properties")
	int32 UpgradeCostLevel3 = 150;

	// Can the tile be purchased?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Properties")
	bool bCanBeBought = true;

	// Can the tile be upgraded?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Properties")
	bool bCanBeUpdate = true;

	// ID of owner (-1 = unowned)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Properties")
	int32 OwnerID = -1;

	// Tile ID on board 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tile Properties")
	int32 TileID = -1;

	// Name displayed on tile
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Properties")
	FString TileName;

	// Current upgrade level 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Properties")
	int32 UpgradeLevel = 0;

	// Rent base multiplier
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Properties")
	float RentRate = 0.2f;

	// Mortgage Interest Rate
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Properties")
	float MortgageInterestRate = 0.1f; // 10% 

	// Mortgage Rate 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Properties")
	float MortgageRate = 0.5f; // 50% 

	// Upgrade rent multipliers
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Properties")
	TArray<float> UpgradeIncomeBonuses = { 0.0f, 0.1f, 0.2f, 0.4f }; // 10%, 20%, 40%

	// Color group for standard tiles
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Properties")
	FString ColorGroup;

	// Special tile category (0=normal, 1=special1, 2=special2) 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Properties")
	int32 SpecialType = 0; 

	// Is the field covered?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Properties")
	bool bIsMortgaged = false;

	// FUNCTIONS
	// Rent without upgrades
	UFUNCTION(BlueprintCallable, Category = "Tile Functions")
	int32 GetBaseRent() const;

	// Rent including upgrades & bonuses 
	UFUNCTION(BlueprintCallable, Category = "Tile Functions")
	int32 GetUpgradedRent() const;

	// Value received for mortgaging tile
	UFUNCTION(BlueprintCallable, Category = "Tile Functions")
	int32 GetMortgageValue() const;

	// Cost to redeem the tile
	UFUNCTION(BlueprintCallable, Category = "Tile Functions")
	int32 GetRedeemCost() const;

	// Attempt to upgrade the tile
	UFUNCTION(BlueprintCallable, Category = "Tile Functions")
	bool UpgradeTile();

	// Rent preview for a given level (does not modify the tile) 
	UFUNCTION(BlueprintCallable, Category = "Tile Functions")
	int32 GetRentForLevel(int32 Level) const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Rendering things
	// Mesh for the tile
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* TileMesh;

	// Stripe mesh for color on main cube
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* StripMesh;

	// Text in widget
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UWidgetComponent* TileWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile UI")
	TSubclassOf<class UUserWidget> TileWidgetClass;

	// Mesh for upgrade levels
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* UpgradeMesh;

private:
	// Global auto-increment tile ID
	static int32 GlobalTileCounter;

	// Updates meshes based on upgrade level
	void UpdateUpgradeMesh();
};
