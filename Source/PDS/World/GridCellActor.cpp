#include <PDS/World/GridCellActor.h>

#include <PDS/WorldObjects/TileType.h>

#include <Components/StaticMeshComponent.h>

AGridCellActor::AGridCellActor()
{
	hex = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HexMesh"));
	hex->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	RootComponent = hex;
}

void AGridCellActor::SetTileType(UTileType* type, UMaterialInterface* overrideMaterial)
{
	hex->SetStaticMesh(type->mesh);
	for (auto i = 0; i < type->materials.Num(); ++i)
		hex->SetMaterial(i, overrideMaterial != nullptr ? overrideMaterial : type->materials[i]);
}
