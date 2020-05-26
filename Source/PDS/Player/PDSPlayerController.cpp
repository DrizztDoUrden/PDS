#include <PDS/Player/PDSPlayerController.h>

#include <PDS/WorldObjects/BuildingType.h>
#include <PDS/World/Grid.h>
#include <PDS/World/GridCellActor.h>
#include <PDS/PDSGameModeBase.h>
#include <PDS/Player/PDSPlayerState.h>
#include <PDS/World/TurnManager.h>

#include <Engine/World.h>
#include <Components/StaticMeshComponent.h>

DEFINE_LOG_CATEGORY_STATIC(PDSPlayerController, Display, All);

APDSPlayerController::APDSPlayerController()
{
	bShowMouseCursor = true;
	PrimaryActorTick.bCanEverTick = true;
}

void APDSPlayerController::InitiateBuildOrder(UBuildingType* buildingType, FName layout)
{
	partialOrder.type = EOrders::Build;
	partialOrder.build.buildingType = buildingType;
	partialOrder.build.layout = layout;

	if (buildingCursor.actor == nullptr)
	{
		const auto world = GetWorld();
		buildingCursor.actor = world->SpawnActor(buildingCursorClass);

		if (buildingCursor.actor == nullptr)
		{
			UE_LOG(PDSPlayerController, Display, TEXT("Failed to spawn a building cursor actor."));
			return;
		}
	}

	const auto mesh = Cast<UStaticMeshComponent>(buildingCursor.actor->GetComponentByClass(UStaticMeshComponent::StaticClass()));
	if (mesh != nullptr)
	{
		mesh->SetStaticMesh(buildingType->mesh);

		if (buildingCursorMaterial != nullptr)
		{
			auto materialIndex = 0;
			for (const auto material : buildingType->materials)
				mesh->SetMaterial(materialIndex++, buildingCursorMaterial);
		}
	}
}

void APDSPlayerController::CancelOrder()
{
	partialOrder.type = EOrders::None;
	if (buildingCursor.actor != nullptr)
	{
		buildingCursor.actor->Destroy();
		buildingCursor.actor = nullptr;
	}
}

void APDSPlayerController::FinishOrder_Implementation(FGridPos target)
{
	if (!ValidateOrder(target))
		return;

	auto task = FTask{};

	switch (partialOrder.type)
	{
	case EOrders::Build:
		task = FBuildTask{target};
		target.CreateBuilingInProcess(this, partialOrder.build.buildingType, partialOrder.build.layout);
		break;
	default:
		checkNoEntry();
	}

	auto state = GetPlayerState<APDSPlayerState>();
	state->tasks.Add(task);
	partialOrder.type = EOrders::None;

	if (buildingCursor.actor != nullptr)
	{
		buildingCursor.actor->Destroy();
		buildingCursor.actor = nullptr;
	}
}

bool APDSPlayerController::ValidateOrder(FGridPos target) const
{
	switch (partialOrder.type)
	{
	case EOrders::Build:
		return target.IsBuildable(partialOrder.build.buildingType);
	default:
		checkNoEntry();
		return false;
	}
}

void APDSPlayerController::FinishTurn_Implementation()
{
	auto mode = Cast<APDSGameModeBase>(GetWorld()->GetAuthGameMode());
	mode->RequestTurnEnd(this);
}

TOptional<FGridPos> APDSPlayerController::GetPosUnderCursor() const
{
	FHitResult hitResult;

	if (!GetHitResultUnderCursor(ECollisionChannel::ECC_GameTraceChannel1, true, hitResult))
		return {};
	if (const auto gca = Cast<AGridCellActor>(hitResult.Actor))
		return gca->pos;
	if (const auto grid = Cast<AGrid>(hitResult.Actor))
		return grid->WorldToPos(hitResult.ImpactPoint);
	return {};
}

void APDSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (GetNetMode() != NM_Client)
	{
		const auto mode = Cast<APDSGameModeBase>(GetWorld()->GetAuthGameMode());
		const auto turn = mode->turnManager->GetTurn();
		BroadcastTurnStart(turn);
	}
}

void APDSPlayerController::TickActor(float deltaTime, enum ELevelTick tickType, FActorTickFunction& thisTickFunction)
{
	Super::TickActor(deltaTime, tickType, thisTickFunction);

	if (GetNetMode() != NM_DedicatedServer && partialOrder.type == EOrders::Build && buildingCursor.actor != nullptr)
		UpdateBuildingCursor();
}

void APDSPlayerController::UpdateBuildingCursor()
{
	const auto hex = GetPosUnderCursor();

	if (!hex || buildingCursor.pos == hex)
		return;

	buildingCursor.pos = hex.GetValue();
	buildingCursor.actor->SetActorLocation(hex->ToWorld() + partialOrder.build.buildingType->offset);
	const auto canBuild = hex->IsBuildable(partialOrder.build.buildingType);

	if (canBuild != buildingCursor.wasError)
	{
		buildingCursor.wasError = canBuild;
		if (canBuild)
			SwitchBuildingCursorToValid();
		else
			SwitchBuildingCursorToInvalid();
	}
}

void APDSPlayerController::SwitchBuildingCursorToValid()
{
	const auto mesh = Cast<UStaticMeshComponent>(buildingCursor.actor->GetComponentByClass(UStaticMeshComponent::StaticClass()));
	if (!mesh || buildingCursorMaterial == nullptr)
		return;
	auto materialIndex = 0;
	for (const auto material : partialOrder.build.buildingType->materials)
		mesh->SetMaterial(materialIndex++, buildingCursorMaterial);
}

void APDSPlayerController::SwitchBuildingCursorToInvalid()
{
	const auto mesh = Cast<UStaticMeshComponent>(buildingCursor.actor->GetComponentByClass(UStaticMeshComponent::StaticClass()));
	if (!mesh || buildingCursorMaterial == nullptr)
		return;
	auto materialIndex = 0;
	for (const auto material : partialOrder.build.buildingType->materials)
		mesh->SetMaterial(materialIndex++, buildingCursorErrorMaterial);
}
