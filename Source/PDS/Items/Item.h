#pragma once

#include <PDS/Utility/ISHelpers.h>

#include <CoreMinimal.h>
#include <Engine/DataAsset.h>

#include "Item.generated.h"

UCLASS(BlueprintType, Blueprintable)
class UItemType : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText displayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float size = 1;
};

USTRUCT(BlueprintType)
struct FItem
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UItemType* type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 amount;
};
