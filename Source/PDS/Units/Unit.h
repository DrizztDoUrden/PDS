#pragma once

#include <PDS/Utility/ISHelpers.h>

#include <CoreMinimal.h>
#include <UObject/Object.h>

#include "Unit.generated.h"

class APDSPlayerState;

UCLASS(BlueprintType, Blueprintable)
class UUnit : public UObject
{
	GENERATED_BODY()

public:
	virtual int32 WorkforceProvided() const { return 1; }
	virtual void OnTurn() {}
	virtual void OnCreated() { if (owner != nullptr) AddToPlayer(); }

	FORCEINLINE_DEBUGGABLE void SetOwner(APDSPlayerState* newOwner)
	{
		if (owner == newOwner) return;
		if (owner != nullptr) RemoveFromPlayer();
		owner = newOwner;
		if (newOwner != nullptr) AddToPlayer();
	}

private:
	UPROPERTY()
	APDSPlayerState* owner;

	virtual void AddToPlayer();
	virtual void RemoveFromPlayer();
};
