#include "MyGameInstance.h"
#include "Blueprint/UserWidget.h"
#include "HubMainWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Components/AudioComponent.h"
#include "MyGameMode.h"

void UMyGameInstance::InitializeHUD(APlayerController* PlayerController)
{
	if (!HUDWidgetClass || !PlayerController)
		return;

	if (!HUDWidgetInstance) {
		HUDWidgetInstance = CreateWidget<UHubMainWidget>(PlayerController, HUDWidgetClass);

		if (HUDWidgetInstance) {
			HUDWidgetInstance->AddToViewport();

			HUDWidgetInstance->UpdateHUDFromGameInstance();

			// Save GameMode Reference
			if (AMyGameMode* GameMode = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(PlayerController))) {
				GameMode->CachedHubWidget = HUDWidgetInstance;
				UE_LOG(LogTemp, Log, TEXT("Set reference to HUD in GameMode."));
			} else {
				UE_LOG(LogTemp, Warning, TEXT("Could not find GameMode!"));
			}
		}
	}
}

void UMyGameInstance::RemoveHUD()
{
	if (HUDWidgetInstance) {
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}
}

void UMyGameInstance::OnStart()
{
    Super::OnStart();

    // We start the music after the world is fully loaded
    EnsureBackgroundMusic();
    UE_LOG(LogTemp, Log, TEXT("MyGameInstance::OnStart() called — music started."));

	FCoreDelegates::ApplicationHasReactivatedDelegate.AddUObject(this, &UMyGameInstance::OnAppReactivate);
}

void UMyGameInstance::EnsureBackgroundMusic()
{
	if (MusicComponent && MusicComponent->IsPlaying())
		return; // already playing - don't restart

	if (!BackgroundMusic) {
		UE_LOG(LogTemp, Warning, TEXT("No assigned music in GameInstance."));
		return;
	}

	UWorld* World = GetWorld();
	if (!World) {
		UE_LOG(LogTemp, Warning, TEXT("No world - can't play music yet."));
		return;
	}

	// Play in loop, volume 0.5
	MusicComponent = UGameplayStatics::SpawnSound2D(World, BackgroundMusic, 0.5f, 1.0f, 0.0f, nullptr, true, false);

	if (MusicComponent) {
		// Run fade-in after start (from volume 0 to 0.5 for 2 seconds)
		MusicComponent->FadeIn(10.0f, 0.5f, 0.0f);
	}

	UE_LOG(LogTemp, Log, TEXT("Background music started (OnStart)."));
}

void UMyGameInstance::OnAppReactivate()
{
	if (MusicComponent && !MusicComponent->IsPlaying()) {
		MusicComponent->Play();
		UE_LOG(LogTemp, Log, TEXT("Music resumed upon returning to the game."));
	}
}

void UMyGameInstance::ToggleBackgroundMusic()
{
	// Is the component present and is the music playing?
	if (MusicComponent && MusicComponent->IsPlaying()) {
		// Slight fade-out and stop
		MusicComponent->FadeOut(1.0f, 0.0f);
		UE_LOG(LogTemp, Log, TEXT("The music has been turned off."));
	} else {
		// if there is no component or it does not work - we restart
		EnsureBackgroundMusic();
		UE_LOG(LogTemp, Log, TEXT("The music turned back on."));
	}
}
