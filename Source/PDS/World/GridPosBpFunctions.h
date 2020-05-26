#pragma once

#include <PDS/World/GridCell.h>
#include <PDS/World/GridPos.h>
#include <PDS/Utility/ISHelpers.h>

#include <CoreMinimal.h>
#include <Kismet/BlueprintFunctionLibrary.h>

#include "GridPosBpFunctions.generated.h"

class UBuilding;

UCLASS()
class UGridPosBPFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (CompactNodeTitle = "Q"))
	static FORCEINLINE_DEBUGGABLE int32 GetQ(FGridPos pos) { return pos.q(); }

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (CompactNodeTitle = "R"))
	static FORCEINLINE_DEBUGGABLE int32 GetR(FGridPos pos) { return pos.r(); }

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (CompactNodeTitle = "Cell"))
	static FORCEINLINE_DEBUGGABLE FGridCell GetCell(FGridPos pos) { return pos.GetCell(); }

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (CompactNodeTitle = "Building"))
	static FORCEINLINE_DEBUGGABLE UBuilding* GetBuilding(FGridPos pos) { return pos.GetBuilding(); }

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (CompactNodeTitle = "World Location"))
	static FORCEINLINE_DEBUGGABLE FVector ToWorld(FGridPos pos) { return pos.ToWorld(); }

	UFUNCTION(BlueprintCallable, BlueprintPure, meta=(CompactNodeTitle = "Stability"))
	static FORCEINLINE_DEBUGGABLE int32 GetStability(FGridPos pos) { return pos.GetStability(); }

	UFUNCTION(BlueprintCallable, BlueprintPure, meta=(CompactNodeTitle = "To String"))
	static FORCEINLINE_DEBUGGABLE FString ToString(FGridPos pos) { return pos.ToString(); }

	UFUNCTION(BlueprintCallable, BlueprintPure, meta=(CompactNodeTitle = "To Text"))
	static FORCEINLINE_DEBUGGABLE FText ToText(FGridPos pos) { return pos.ToText(); }
};
