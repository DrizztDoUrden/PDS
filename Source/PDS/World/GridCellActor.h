#pragma once

#include <PDS/World/GridPos.h>
#include <PDS/Utility/ISHelpers.h>

#include <CoreMinimal.h>
#include <GameFramework/Actor.h>

#include "GridCellActor.generated.h"

class UStaticMeshComponent;
class UTileType;

UCLASS(Blueprintable, BlueprintType)
class AGridCellActor : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	FGridPos pos;

	AGridCellActor();

	void SetTileType(UTileType* type, UMaterialInterface* overrideMaterial = nullptr);

private:
	UPROPERTY()
	UStaticMeshComponent* hex;

	UTileType* tileType;
};
