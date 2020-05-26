#include <PDS/World/Building.h>

#include <PDS/Player/PDSPlayerState.h>
#include <PDS/WorldObjects/BuildingType.h>

void UBuilding::SetLayout(FName newLayout)
{
	if (layoutName == newLayout)
		return;

	if (owner && !newLayout.IsNone())
	{
		owner->inventoryCapacity -= GetLayout().inventoryCapacity;
		owner->livingCapacity -= GetLayout().livingCapacity;
		owner->workshopCapacity -= GetLayout().workshopCapacity;
	}

	layoutName = newLayout;

	if (owner && !newLayout.IsNone())
	{
		owner->inventoryCapacity += GetLayout().inventoryCapacity;
		owner->livingCapacity += GetLayout().livingCapacity;
		owner->workshopCapacity += GetLayout().workshopCapacity;
	}
}

void UBuilding::SetOwner(APDSPlayerState* const newOwner)
{
	if (owner) owner->buildings.Remove(this);
	IDynamicInventoryHolder::SetOwner(this, owner, inventory, newOwner);
	if (newOwner) newOwner->buildings.Remove(this);
}
