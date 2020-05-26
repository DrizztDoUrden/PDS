#pragma once

#include <PDS/WorldObjects/WorldObject.h>
#include <PDS/Utility/ISHelpers.h>

#include <CoreMinimal.h>

#include "CellContent.generated.h"

UCLASS(BlueprintType)
class UCellContent : public UWorldObject
{
	GENERATED_BODY()

public:
	FORCEINLINE_DEBUGGABLE bool IsPreventingBuilding() const { return preventsBuilding; }

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	bool preventsBuilding;
};
