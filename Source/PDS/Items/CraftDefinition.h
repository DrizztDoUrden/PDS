#pragma once

#include <PDS/Items/Item.h>
#include <PDS/Utility/ISHelpers.h>

#include <CoreMinimal.h>
#include <Engine/DataAsset.h>

#include "CraftDefinition.generated.h"

UCLASS(BlueprintType)
class UCraftDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FItem> sources;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FItem> instruments;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FItem> result;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 workforceLimit;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 workforceRequired;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FName> workspaceRequirements;
};
