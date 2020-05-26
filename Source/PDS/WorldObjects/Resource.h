#pragma once

#include <PDS/Items/Item.h>
#include <PDS/WorldObjects/CellContent.h>
#include <PDS/Utility/ISHelpers.h>

#include <CoreMinimal.h>

#include "Resource.generated.h"

UCLASS(BlueprintType)
class UResource : public UCellContent
{
	GENERATED_BODY()

public:
	FORCEINLINE_DEBUGGABLE const TArray<FItem>& GetProductsPerCicle() const { return productsPerCicle; }

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess = true))
	TArray<FItem> productsPerCicle;
};
