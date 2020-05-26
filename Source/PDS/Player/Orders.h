#pragma once

#include <CoreMinimal.h>

#include "Orders.generated.h"

UENUM(BlueprintType)
enum class EOrders : uint8
{
	None,
	Build,
	Craft,
};
