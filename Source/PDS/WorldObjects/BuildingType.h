#pragma once

#include <PDS/Items/Item.h>
#include <PDS/World/GridPos.h>
#include <PDS/WorldObjects/WorldObject.h>
#include <PDS/Utility/ISHelpers.h>

#include <CoreMinimal.h>

#include "BuildingType.generated.h"

USTRUCT(BlueprintType)
struct FBuildingLayout
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText displayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 inventoryCapacity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 livingCapacity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 workshopCapacity;
};

class AGrid;
class APDSPlayerState;
struct FGridCell;
class UTileType;

USTRUCT(BlueprintType)
struct FBuildStep
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FItem> cost;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 workforce;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 workforceLimit;
};

UCLASS(Blueprintable, BlueprintType)
class UBuildingType : public UWorldObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<FName, FBuildingLayout> layouts;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<UTileType*> tilesCanBeBuiltOn;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FBuildStep> builingSteps;

	bool IsBuildable(const APDSPlayerState* player) const;
	bool IsBuildable(FGridPos pos) const;
	FORCEINLINE_DEBUGGABLE bool IsBuildable(const APDSPlayerState* player, FGridPos pos) const { return IsBuildable(player) && IsBuildable(pos); }

	virtual void OnBuilt(FGridPos hex) const;
	virtual void OnTurn(FGridPos hex) const;
	virtual void OnStabilityChange(FGridPos hex, int32 oldStability) const {}
	virtual void OnTileTypeChange(FGridPos hex, UTileType* oldTile) const {}
};
