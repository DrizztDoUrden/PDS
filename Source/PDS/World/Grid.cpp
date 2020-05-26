#include <PDS/World/Grid.h>

#include <PDS/Player/PDSPlayerState.h>
#include <PDS/Player/PDSPlayerController.h>
#include <PDS/World/Building.h>
#include <PDS/World/GridCellActor.h>
#include <PDS/WorldObjects/BuildingType.h>
#include <PDS/WorldObjects/TileType.h>
#include <PDS/WorldObjects/WorldObject.h>

#include <Components/InstancedStaticMeshComponent.h>
#include <Engine/World.h>

DEFINE_LOG_CATEGORY_STATIC(Grid, Display, All);

AGrid::AGrid()
{
	SetReplicates(true);
	cellActor = AGridCellActor::StaticClass();
}

void AGrid::SpawnTile(Hexes::Int pos, UTileType* tileType)
{
	if(!tileType)
		UE_LOG(Grid, Fatal, TEXT("No tile type provided to SpawnTile."));

	auto& cell = cells.FindOrAdd(pos);
	if (cell.tile == tileType)
		return;
	if (!cell.actor.IsValid())
		cell.actor = SpawnCellActor(pos, tileType);

	const auto old = cell.tile;
	cell.tile = tileType;
	cell.actor->SetTileType(tileType);
	if (cell.building)
		cell.building->GetType()->OnTileTypeChange({this, pos}, old);
}

UBuilding* AGrid::Build(Hexes::Int pos, UBuildingType* buildingType, FName layoutName, APDSPlayerState* owner)
{
	if (owner)
	{
		check(buildingType->IsBuildable(owner, { this, pos }));
	}
	else
	{
		check(buildingType->IsBuildable({ this, pos }));
	}

	auto& cell = cells[pos];
	const auto meshes = GetBuildingsISM(buildingType);
	const auto id = meshes->AddInstance(FTransform{ HexToWorld(pos) + buildingType->offset });

	if (!cell.HasBuilding())
		cell.building = NewObject<UBuilding>(this);
	const auto markerId = cell.building->SetBuilding(id, buildingType);
	if (markerId != -1)
		FreeBuildingMarker(markerId);

	const auto layoutName_ = !layoutName.IsNone() ? layoutName : cell.building->GetLayoutName();
	cell.building->SetLayout(layoutName_);
	cell.building->SetOwner(owner = owner ? owner : cell.building->GetOwner());
	buildingType->OnBuilt({this, pos});

	return cell.building;
}

void AGrid::CreateBuilingInProcess(APDSPlayerController* owner, Hexes::Int pos, UBuildingType* buildingType, FName layoutName)
{
	const auto state = Cast<APDSPlayerState>(owner->PlayerState);

	check(buildingType->IsBuildable(state, { this, pos }));

	auto& cell = cells[pos];

	if (cell.HasBuilding() && cell.building->IsMarker())
		return;

	const auto pair = AllocateBuildingMarker(pos, buildingType);
	cell.building = NewObject<UBuilding>(this);
	cell.building->SetMarker(buildingType, pair.Key, state, layoutName);
}

void AGrid::SwitchBuildingLayout(Hexes::Int pos, FName newLayout)
{
	auto& cell = cells[pos];
	check(cell.HasBuilding());
	cell.building->SetLayout(newLayout);
}

UInstancedStaticMeshComponent* AGrid::LoadOrConstructISM(TMap<FName, UInstancedStaticMeshComponent*>& category, const UWorldObject* objectType, UMaterialInterface* overrideMaterial)
{
	const auto name = objectType->name;
	if (buildingMeshes.Contains(name))
		return buildingMeshes[name];

	const auto meshes = NewObject<UInstancedStaticMeshComponent>(this);

	meshes->AttachToComponent(RootComponent, { EAttachmentRule::SnapToTarget, true });
	meshes->SetStaticMesh(objectType->mesh);
	for (auto i = 0; i < objectType->materials.Num(); ++i)
		meshes->SetMaterial(i, overrideMaterial != nullptr ? overrideMaterial : objectType->materials[i]);
	meshes->SetMobility(EComponentMobility::Static);
	meshes->SetIsReplicated(true);

	meshes->OnComponentCreated();
	meshes->RegisterComponent();
	if (meshes->bWantsInitializeComponent)
		meshes->InitializeComponent();

	return meshes;
}

TPair<int32, UStaticMeshComponent*> AGrid::AllocateBuildingMarker(Hexes::Int pos, UBuildingType* buildingType)
{
	const auto id = [&]() {
		if (unusedBuildingMarkers.Num() <= 0)
			return -1;

		const auto ret = unusedBuildingMarkers.Pop();
		return ret;
	}();

	const auto mesh = NewObject<UStaticMeshComponent>(this);
	mesh->OnComponentCreated();
	mesh->RegisterComponent();
	if (mesh->bWantsInitializeComponent)
		mesh->InitializeComponent();
	mesh->AttachToComponent(RootComponent, { EAttachmentRule::SnapToTarget, true });
	mesh->SetStaticMesh(buildingType->mesh);
	for (auto i = 0; i < buildingType->materials.Num(); ++i)
		mesh->SetMaterial(i, buildingMarkerMaterial);
	mesh->SetWorldLocation(HexToWorld(pos) + buildingType->offset);
	mesh->SetMobility(EComponentMobility::Static);

	if (id == -1)
		return TPair<int32, UStaticMeshComponent*>{buildingMarkers.Add(mesh), mesh};

	buildingMarkers[id] = mesh;
	return TPair<int32, UStaticMeshComponent*>{id, mesh};
}

void AGrid::FreeBuildingMarker(int32 id)
{
	buildingMarkers[id]->DestroyComponent();
	unusedBuildingMarkers.Add(id);
}

AGridCellActor* AGrid::SpawnCellActor(Hexes::Int pos, UTileType* tileType)
{
	const auto wPos = HexToWorld(pos) + tileType->offset;
	const auto wTransform = FTransform{ FRotator{0, -30 + 60 * layout.orientation.start_angle, 0}, wPos };
	const auto world = GetWorld();
	auto actor = world->SpawnActorDeferred<AGridCellActor>(cellActor, wTransform);
	actor->pos = { this, pos };
	actor->FinishSpawning(wTransform);
	return actor;
}
