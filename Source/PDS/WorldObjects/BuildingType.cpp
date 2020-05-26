#include <PDS/WorldObjects/BuildingType.h>

#include <PDS/Player/PDSPlayerState.h>
#include <PDS/World/Building.h>

bool UBuildingType::IsBuildable(const APDSPlayerState* player) const
{
	return true;
}

bool UBuildingType::IsBuildable(FGridPos pos) const
{
	const auto hasMarkerOfThis = pos.GetBuilding() && pos.GetBuilding()->IsMarker() && pos.GetBuilding()->GetType() == this;
	const auto canBeBuiltOnTile = tilesCanBeBuiltOn.Num() == 0 || tilesCanBeBuiltOn.Contains(pos.GetTileType());
	return (pos.IsEmptyForBuilding() || hasMarkerOfThis) && canBeBuiltOnTile;
}

void UBuildingType::OnBuilt(FGridPos hex) const
{
	const auto building = hex.GetBuilding();
	const auto owner = building->GetOwner();
	building->pos = hex;

	if (owner != nullptr)
	{
		owner->inventoryCapacity += building->GetLayout().inventoryCapacity;
		owner->livingCapacity += building->GetLayout().livingCapacity;
		owner->workshopCapacity += building->GetLayout().workshopCapacity;
		owner->buildings.Add(building);
	}
}

void UBuildingType::OnTurn(FGridPos hex) const
{
	hex.GetBuilding()->OnTurn();
}
