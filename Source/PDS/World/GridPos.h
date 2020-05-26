#pragma once

#include <PDS/Hexes/Hexes.h>
#include <PDS/Utility/ISHelpers.h>

#include <CoreMinimal.h>
#include <Kismet/BlueprintFunctionLibrary.h>

#include "GridPos.generated.h"

class AGrid;
class APDSPlayerController;
class APDSPlayerState;
struct FGridCell;
class UBuilding;
class UBuildingType;
class UTileType;

USTRUCT(BlueprintType)
struct FGridPos
{
	GENERATED_BODY()

public:
	FORCEINLINE_DEBUGGABLE FGridPos() = default;
	FORCEINLINE_DEBUGGABLE FGridPos(AGrid* grid_, Hexes::Int pos_) : grid(grid_), pos(pos_) {}

	FORCEINLINE_DEBUGGABLE AGrid* GetGrid() const { return grid; }
	FORCEINLINE_DEBUGGABLE Hexes::Int GetHex() const { return pos; }
	FORCEINLINE_DEBUGGABLE int32 q() const { return Pos().q; }
	FORCEINLINE_DEBUGGABLE int32 r() const { return Pos().r; }
	FORCEINLINE_DEBUGGABLE int32 DistanceTo(FGridPos other) const { return Pos().Distance(other.pos); }
	FORCEINLINE_DEBUGGABLE int32 DistanceTo(Hexes::Int other) const { return Pos().Distance(other); }
	UTileType* GetTileType() const;
	bool Exists() const;
	void SpawnTile(UTileType* tileType) const;
	FGridCell& GetCell() const;
	UBuilding* GetBuilding() const;
	FVector ToWorld() const;
	UBuilding* Build(UBuildingType* buildingType = nullptr, FName layoutName = NAME_None, APDSPlayerState* owner = nullptr) const;
	int32 GetStability() const;
	int32 GetStability(Hexes::Int source) const;
	void SetStability(Hexes::Int source, int32 value) const;
	void CreateBuilingInProcess(APDSPlayerController* owner, UBuildingType* buildingType, FName layoutName) const;
	bool IsBuildable(UBuildingType* building) const;
	bool HasBuilding() const;
	bool IsEmptyForBuilding() const;
	FString ToString() const;
	FORCEINLINE_DEBUGGABLE FText ToText() const { return FText::FromString(ToString()); }

	FORCEINLINE_DEBUGGABLE bool operator == (FGridPos other) const { return grid == other.grid && pos == other.pos; }
	FORCEINLINE_DEBUGGABLE bool operator != (FGridPos other) const { return !(*this == other); }
	FORCEINLINE_DEBUGGABLE operator Hexes::Int() const { return Pos(); }

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = true))
	AGrid* grid = nullptr;

	UPROPERTY()
	FIntPoint pos;

	FORCEINLINE_DEBUGGABLE Hexes::Int Pos() const { return pos; }
};
