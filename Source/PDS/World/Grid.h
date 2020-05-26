#pragma once

#include <PDS/World/GridCell.h>
#include <PDS/World/GridPos.h>
#include <PDS/Hexes/Hexes.h>
#include <PDS/Utility/Iterators.h>
#include <PDS/Utility/ISHelpers.h>

#include <CoreMinimal.h>
#include <Engine/DataAsset.h>
#include <GameFramework/Actor.h>

#include "Grid.generated.h"

class UInstancedStaticMeshComponent;
class UStaticMeshComponent;

class AGridCellActor;
class APDSPlayerController;
class APDSPlayerState;
class UBuilding;
class UBuildingType;
class UTileType;
class UWorldObject;

// Most of grid APIs should be used through FGridPos when applicable. That's more compact and safe.
UCLASS(BlueprintType, Blueprintable)
class AGrid : public AActor
{
	GENERATED_BODY()

public:
	AGrid();

	FORCEINLINE_DEBUGGABLE const auto& GetCells() const { return cells; }

	void SpawnTile(Hexes::Int pos, UTileType* tileType);
	UBuilding* Build(Hexes::Int pos, UBuildingType* buildingType, FName layoutName = NAME_None, APDSPlayerState* owner = nullptr);
	void CreateBuilingInProcess(APDSPlayerController* owner, Hexes::Int pos, UBuildingType* buildingType, FName layoutName);
	void SwitchBuildingLayout(Hexes::Int pos, FName newLayout);

	FORCEINLINE_DEBUGGABLE FVector HexToWorld(Hexes::Int hex) const { return FVector{ hex.ToPixel(layout), 0 }; }
	FORCEINLINE_DEBUGGABLE Hexes::Int WorldToHex(FVector v) const { return Hexes::Float{ layout, {v.X, v.Y} }.Round(); }
	FORCEINLINE_DEBUGGABLE FGridPos WorldToPos(FVector v) { return {this, WorldToHex(v)}; }
	FORCEINLINE_DEBUGGABLE FGridCell& GetCell(Hexes::Int hex) { return cells[hex]; }
	FORCEINLINE_DEBUGGABLE const FGridCell& GetCell(Hexes::Int hex) const { return cells[hex]; }
	FORCEINLINE_DEBUGGABLE bool HasCell(Hexes::Int hex) const { return cells.Contains(hex); }

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="BuildingMarker", meta = (AllowPrivateAccess=true))
	UMaterialInterface* buildingMarkerMaterial;

	UPROPERTY()
	TMap<FName, UInstancedStaticMeshComponent*> cellMeshes;
	UPROPERTY()
	TMap<FName, UInstancedStaticMeshComponent*> buildingMeshes;
	UPROPERTY()
	TArray<UStaticMeshComponent*> buildingMarkers;
	TArray<int32> unusedBuildingMarkers;

	TMap<Hexes::Int, FGridCell> cells;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AGridCellActor> cellActor;

	Hexes::Layout layout = Hexes::Layout{ Hexes::Orientation::Pointy(), {100, 100} };

	UInstancedStaticMeshComponent* LoadOrConstructISM(TMap<FName, UInstancedStaticMeshComponent*>& category, const UWorldObject* objectType, UMaterialInterface* overrideMaterial = nullptr);
	TPair<int32, UStaticMeshComponent*> AllocateBuildingMarker(Hexes::Int pos, UBuildingType* buildingType);
	void FreeBuildingMarker(int32 id);

	FORCEINLINE_DEBUGGABLE UInstancedStaticMeshComponent* GetCellsISM(const UWorldObject* objectType) { return LoadOrConstructISM(cellMeshes, objectType); }
	FORCEINLINE_DEBUGGABLE UInstancedStaticMeshComponent* GetBuildingsISM(const UWorldObject* objectType) { return LoadOrConstructISM(buildingMeshes, objectType); }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UBuilding* GetBuildingAt(FIntPoint hex) { return GetCell(hex).building; }

	virtual AGridCellActor* SpawnCellActor(Hexes::Int pos, UTileType* tileType);
};
