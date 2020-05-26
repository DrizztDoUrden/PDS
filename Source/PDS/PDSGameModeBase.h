#pragma once

#include <PDS/Items/Item.h>
#include <PDS/World/GridPos.h>

#include <CoreMinimal.h>
#include <GameFramework/GameModeBase.h>

#include "PDSGameModeBase.generated.h"

class AGrid;
class ATurnManager;
class APDSPlayerController;
class UBuildingType;
class UCraftDefinition;
class UItemType;
class UTileType;
class UStabilizer;

using namespace UC;
using namespace UP;
using namespace UF;
using namespace UM;

UCLASS(Blueprintable, BlueprintType)
class PDS_API APDSGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Classes")
	TSubclassOf<AGrid> gridClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Classes")
	TSubclassOf<ATurnManager> turnManagerClass;

	TArray<AGrid*> grids;
	ATurnManager* turnManager;

	UPROPERTY(BlueprintReadOnly)
	bool turnInProgress = false;

	APDSGameModeBase();

	void RequestTurnEnd(APDSPlayerController* player);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Starting")
	UStabilizer* portal;

	UPROPERTY(EditDefaultsOnly, Category = "Starting")
	UBuildingType* wagon;

	UPROPERTY(EditDefaultsOnly, Category = "Starting")
	TArray<FItem> startingItems;

	UPROPERTY(EditDefaultsOnly, Category = "Starting")
	int32 startingUnits = 8;

	UPROPERTY(EditDefaultsOnly, Category = "Starting")
	TArray<UCraftDefinition*> startingCrafts;

	UPROPERTY(EditDefaultsOnly, Category = "Test Tiles")
	UTileType* bloodStone;

	void BeginPlay() override;

	virtual void GenerateGrid();
	virtual void GenerateStartingPortal(FGridPos pos);
	virtual void GeneratePlayerStarts();
	virtual void GeneratePlayerStart(APDSPlayerController* player, FGridPos pos);
};
