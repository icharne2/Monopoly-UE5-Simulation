#include "ABoardTile.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h" 
#include "UObject/ConstructorHelpers.h"
#include "MyGameMode.h"
#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

AABoardTile::AABoardTile() // constructor
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//Create main mesh cube
	TileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TileMesh"));
	RootComponent = TileMesh;
	TileMesh->SetWorldScale3D(FVector(1.0f, 1.0f, 0.2f));

	//Create addictional cube - StripMesh
	StripMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StripMesh"));
	StripMesh->SetupAttachment(TileMesh);
	StripMesh->SetRelativeLocation(FVector(0.0f, 40.0f, 51.0f));
	StripMesh->SetWorldScale3D(FVector(1.0f, 0.2f, 0.01f));

	//Mesh for upgrades
	UpgradeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("UpgradeMesh"));
	UpgradeMesh->SetupAttachment(TileMesh);
	UpgradeMesh->SetRelativeLocation(FVector(30.0f, 30.0f, 41.0f));
	UpgradeMesh->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	UpgradeMesh->SetWorldScale3D(FVector(0.5f, 0.5f, 1.5f));
	UpgradeMesh->SetVisibility(false);

	//Text in widget 
	TileWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("TileWidget"));
	TileWidget->SetupAttachment(TileMesh);

	TileWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 51.0f)); 
	TileWidget->SetRelativeRotation(FRotator(90.0f, 0.0f, -90.0f)); 

	TileWidget->SetDrawSize(FVector2D(300.0f, 400.0f));
	TileWidget->SetRelativeScale3D(FVector(0.5f, 0.3f, 1.0f));  
	TileWidget->SetPivot(FVector2D(0.5f, 0.5f));

	TileWidget->SetWidgetSpace(EWidgetSpace::World);
	TileWidget->SetVisibility(true);
}

void AABoardTile::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void AABoardTile::BeginPlay()
{
	Super::BeginPlay();

	if (TileWidget) {
		if (UUserWidget* Widget = TileWidget->GetUserWidgetObject()) {
				// Assign tile name to the widget TextBlock using reflection,
				// because the TileNameWBP variable cannot be accessed normally.
			if (FProperty* Property = Widget->GetClass()->FindPropertyByName(FName("TileNameWBP"))) {
				void* PropAddr = Property->ContainerPtrToValuePtr<void>(Widget);
				FText NameText = FText::FromString(TileName);
				*(FText*)PropAddr = NameText;
			}
		}
	}

	//Afrer upgrade
	UpdateUpgradeMesh();
}

void AABoardTile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

int32 AABoardTile::GetBaseRent() const
{
	return FMath::RoundToInt(TileCost * RentRate);
}

int32 AABoardTile::GetUpgradedRent() const
{
	float BaseRent = GetBaseRent();

	// For regular fields if they have an upgrade
	if (SpecialType == 0 && UpgradeLevel >= 0 && UpgradeLevel < UpgradeIncomeBonuses.Num()) {
		float Bonus = UpgradeIncomeBonuses[UpgradeLevel];
		return FMath::RoundToInt(BaseRent * (1.0f + Bonus));
	}

	// For special fields type 1 and 2
	if (SpecialType == 1 || SpecialType == 2) {
		AMyGameMode* GM = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

		if (GM) {
			APlayerCharacter* OwnerPlayer = GM->GetPlayerByID(OwnerID);
			if (OwnerPlayer) {
				float Multiplier = (SpecialType == 1)
					? OwnerPlayer->RentBonusType1
					: OwnerPlayer->RentBonusType2;

				return FMath::RoundToInt(BaseRent * Multiplier);
			}
		}
	}

	return BaseRent;
}

int32 AABoardTile::GetRentForLevel(int32 Level) const
{
	int32 BaseRent = GetBaseRent();

	if (Level >= 0 && Level < UpgradeIncomeBonuses.Num()) {
		float Bonus = UpgradeIncomeBonuses[Level];

		return FMath::RoundToInt(BaseRent * (1.0f + Bonus));
	}

	return BaseRent;
}

int32 AABoardTile::GetMortgageValue() const
{
	return FMath::RoundToInt(TileCost * MortgageRate);
}

int32 AABoardTile::GetRedeemCost() const
{
	return FMath::RoundToInt(TileCost * MortgageRate * (1.0f + MortgageInterestRate));
}

bool AABoardTile::UpgradeTile()
{
	// Upgrade allowed only if next level exists and field can be upgraded
	if (UpgradeLevel + 1 < UpgradeIncomeBonuses.Num() && bCanBeUpdate == true) {
		UpgradeLevel++;

		// Odśwież po jednej klatce (bezpieczne w runtime)
		FTimerHandle RefreshHandle;
		GetWorldTimerManager().SetTimer(RefreshHandle, [this]() { UpdateUpgradeMesh(); }, 0.05f, false);

		return true;
	}
	return false;
}

// Update mesh 
void AABoardTile::UpdateUpgradeMesh()
{
	if (!UpgradeMesh)
		return;

	if (UpgradeLevel == 0) {
		UpgradeMesh->SetVisibility(false);
		return;
	}

	UpgradeMesh->SetVisibility(true);

	// Load mesh dynamically (this requires exact path stored in project)
	FString Path;
	switch (UpgradeLevel) {
		case 1:
			Path = TEXT("/Game/Meshes/Updates/SM_Upgrade1.SM_Upgrade1");
			break;
		case 2:
			Path = TEXT("/Game/Meshes/Updates/SM_Upgrade2.SM_Upgrade2");
			break;
		case 3:
			Path = TEXT("/Game/Meshes/Updates/SM_Upgrade3.SM_Upgrade3");
			break;
		default:
			UpgradeMesh->SetVisibility(false);
			return;
	}

	//Load mesh
	UStaticMesh* LoadedMesh = LoadObject<UStaticMesh>(nullptr, *Path);
	if (LoadedMesh) {
		UpgradeMesh->SetStaticMesh(LoadedMesh);

		switch (UpgradeLevel) {
			case 2:
				UpgradeMesh->SetRelativeLocation(FVector(30.0f, 30.0f, 100.0f));
				UpgradeMesh->SetWorldScale3D(FVector(0.005f, 0.005f, 0.005f));
				UpgradeMesh->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
				break;
			case 3:
				UpgradeMesh->SetRelativeLocation(FVector(30.0f, 30.0f, 130.0f));
				UpgradeMesh->SetWorldScale3D(FVector(0.3f, 0.3f, 0.5f));  
				UpgradeMesh->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
				break;
		}
	} else {
		UpgradeMesh->SetVisibility(false);
	}
}
