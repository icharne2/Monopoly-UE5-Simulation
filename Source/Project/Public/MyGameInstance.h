#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Blueprint/UserWidget.h"
#include "MyGameInstance.generated.h"

class UHubMainWidget;
class USoundBase;
class UAudioComponent;

UCLASS()
class PROJECT_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	// Start game
	virtual void OnStart() override;

	// Final ranking data
	UPROPERTY(BlueprintReadWrite)
	TArray<FString> FinalPlayerNames;

	UPROPERTY(BlueprintReadWrite)
	TArray<int32> FinalScores;

	UPROPERTY(BlueprintReadWrite)
	TArray<int32> FinalRanks;

	// Player names - WBP_StartGame (start game widget)
	UPROPERTY(BlueprintReadWrite, Category = "Player Data")
	FString Player1Name;

	UPROPERTY(BlueprintReadWrite, Category = "Player Data")
	FString Player2Name;

	UPROPERTY(BlueprintReadWrite, Category = "Player Data")
	FString Player3Name;

	UPROPERTY(BlueprintReadWrite, Category = "Player Data")
	FString Player4Name;


	// HUB Pannel
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UHubMainWidget> HUDWidgetClass;

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	class UHubMainWidget* HUDWidgetInstance;

	UFUNCTION(BlueprintCallable, Category = "UI")
	void InitializeHUD(APlayerController* PlayerController);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void RemoveHUD();

	// Background music
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* BackgroundMusic;

	UPROPERTY(BlueprintReadWrite, Category = "Audio")
	UAudioComponent* MusicComponent;

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void ToggleBackgroundMusic();

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void EnsureBackgroundMusic();

	// Called when app regains focus
	UFUNCTION()
	void OnAppReactivate();
};
