#pragma once

#include <PDS/Hexes/Hexes.h>
#include <PDS/Utility/ISHelpers.h>

#include <CoreMinimal.h>

#include "GridCell.generated.h"

class AGridCellActor;
class UBuilding;
class UCellContent;
class UTileType;

USTRUCT(BlueprintType)
struct FCellContentSlot
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	UCellContent* type;

	UPROPERTY(BlueprintReadOnly)
	int32 ammount;
};

USTRUCT(BlueprintType)
struct FGridCell
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	UTileType* tile;

	UPROPERTY(BlueprintReadOnly)
	UBuilding* building;

	UPROPERTY(BlueprintReadOnly)
	TArray<FCellContentSlot> content;

	UPROPERTY()
	TMap<FIntPoint, int32> stabilityBonuses;

	UPROPERTY()
	TWeakObjectPtr<AGridCellActor> actor = nullptr;

	FORCEINLINE_DEBUGGABLE bool HasBuilding() const { return building != nullptr; }
};
