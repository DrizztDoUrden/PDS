#pragma once

#include <CoreMinimal.h>
#include <Engine/DataAsset.h>

#include "WorldObject.generated.h"

using namespace UC;
using namespace UP;
using namespace UF;
using namespace UM;

class UStaticMesh;
class UMaterialInterface;

UCLASS(NotBlueprintable, NotBlueprintType)
class UWorldObject : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText displayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMesh* mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<UMaterialInterface*> materials;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector offset = FVector::ZeroVector;
};
