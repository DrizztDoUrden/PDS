#pragma once

#include <PDS/Items/Item.h>
#include <PDS/Utility/ISHelpers.h>

#include <CoreMinimal.h>
#include <UObject/Interface.h>

#include "InventoryHolder.generated.h"

class APDSPlayerState;

UINTERFACE(BlueprintType, meta=(CannotImplementInterfaceInBlueprint))
class UInventoryHolder : public UInterface
{
	GENERATED_BODY()
};

class IInventoryHolder
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	virtual const TArray<FItem>& GetInventory() const PURE_VIRTUAL(GetInventory, static TArray<FItem> v; return v;);
};

UINTERFACE(BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class UDynamicInventoryHolder : public UInventoryHolder
{
	GENERATED_BODY()
};

class IDynamicInventoryHolder : public IInventoryHolder
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	virtual void AddItem(const FItem& item) PURE_VIRTUAL(AddItem, );

	UFUNCTION(BlueprintCallable)
	virtual void RemoveItem(const FItem& item) PURE_VIRTUAL(RemoveItem, );

protected:
	static void SetOwner(TScriptInterface<IDynamicInventoryHolder> self, APDSPlayerState*& owner, TArray<FItem>& inventory, APDSPlayerState* newOwner);
	static void AddItem(APDSPlayerState* owner, TArray<FItem>& inventory, const FItem& item);
	static void RemoveItem(APDSPlayerState* owner, TArray<FItem>& inventory, const FItem& item);
};

class APDSPlayerState;
