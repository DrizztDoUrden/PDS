#include <PDS/Items/InventoryHolder.h>

#include <PDS/Player/PDSPlayerState.h>

void IDynamicInventoryHolder::SetOwner(TScriptInterface<IDynamicInventoryHolder> self, APDSPlayerState*& owner, TArray<FItem>& inventory, APDSPlayerState* const newOwner)
{
	if (owner == newOwner)
		return;

	if (owner != nullptr)
	{
		owner->inventories.Remove(self);
		for (const auto& item : inventory)
			owner->UnregisterItem(item);
	}

	owner = newOwner;

	if (newOwner != nullptr)
	{
		owner->inventories.Add(self);
		for (const auto& item : inventory)
			newOwner->RegisterItem(item);
	}
}

void IDynamicInventoryHolder::AddItem(APDSPlayerState* owner, TArray<FItem>& inventory, const FItem& item)
{
	if (owner != nullptr)
		owner->RegisterItem(item);

	for (auto& invItem : inventory)
	{
		if (invItem.type != item.type)
			continue;

		invItem.amount += item.amount;
		return;
	}

	inventory.Add(item);
}

void IDynamicInventoryHolder::RemoveItem(APDSPlayerState* owner, TArray<FItem>& inventory, const FItem& item)
{
	for (auto id = 0; id < inventory.Num(); ++id)
	{
		auto& invItem = inventory[id];
		if (invItem.type != item.type)
			continue;

		if (owner != nullptr)
		{
			auto removed = item;
			removed.amount = FMath::Min(item.amount, invItem.amount);
			owner->UnregisterItem(removed);
		}

		invItem.amount -= item.amount;
		if (invItem.amount <= 0)
			inventory.RemoveAt(id);
		return;
	}
}
