#include <PDS/World/GridPos.h>

#include <PDS/World/Building.h>
#include <PDS/World/Grid.h>
#include <PDS/World/GridCellActor.h>
#include <PDS/WorldObjects/BuildingType.h>
#include <PDS/WorldObjects/CellContent.h>

#include <Algo/AnyOf.h>
#include <Algo/Accumulate.h>

UTileType* FGridPos::GetTileType() const { return GetCell().tile; }
bool FGridPos::Exists() const { check(grid); return grid->HasCell(pos); }
void FGridPos::SpawnTile(UTileType* tileType) const { check(grid); grid->SpawnTile(pos, tileType); }
FGridCell& FGridPos::GetCell() const { check(grid); return grid->GetCell(pos); }
UBuilding* FGridPos::GetBuilding() const { return GetCell().building; }
FVector FGridPos::ToWorld() const { check(grid); return grid->HexToWorld(pos); }

UBuilding* FGridPos::Build(UBuildingType* buildingType, FName layout, APDSPlayerState* owner) const
{
    const auto building = GetBuilding();
    check(building || buildingType);
    const auto effectiveBuildingType = buildingType ? buildingType : building->GetType();
    const auto effectiveLayout = !layout.IsNone() ? layout : effectiveBuildingType->layouts.begin()->Key;
    const auto effectiveOwner = owner ? owner : building ? building->GetOwner() : nullptr;
    return grid->Build(pos, effectiveBuildingType, effectiveLayout, effectiveOwner);
}

int32 FGridPos::GetStability() const
{
    return Algo::Accumulate(GetCell().stabilityBonuses, 0, [](auto v, auto kvp) { return v + kvp.Value; });
}

int32 FGridPos::GetStability(Hexes::Int source) const
{
    const auto bonus = GetCell().stabilityBonuses.Find(source);
    return bonus ? *bonus : 0;
}

void FGridPos::SetStability(Hexes::Int source, int32 value) const
{
    auto& cell = GetCell();
    const auto old = GetStability(source);

    if (value == old)
        return;

    if (value != 0)
        cell.stabilityBonuses.FindOrAdd(source) = value;
    else
        cell.stabilityBonuses.Remove(source);

    if (cell.building)
        cell.building->GetType()->OnStabilityChange(*this, old);
}

void FGridPos::CreateBuilingInProcess(APDSPlayerController* owner, UBuildingType* buildingType, FName layoutName) const
{
    check(grid);
    grid->CreateBuilingInProcess(owner, pos, buildingType, layoutName);
}

bool FGridPos::IsBuildable(UBuildingType* building) const
{
    return building->IsBuildable(*this);
}

bool FGridPos::HasBuilding() const
{
    return GetCell().building && !GetCell().building->IsMarker();
}

bool FGridPos::IsEmptyForBuilding() const
{
    return !GetCell().building && !Algo::AnyOf(GetCell().content, [](const FCellContentSlot& slot) { return slot.type->IsPreventingBuilding(); });
}

FString FGridPos::ToString() const
{
    return FString::Printf(TEXT("(%s:%d;%d)"), *grid->GetActorLabel(), q(), r());
}
