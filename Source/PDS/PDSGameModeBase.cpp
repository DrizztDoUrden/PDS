#include <PDS/PDSGameModeBase.h>

#include <PDS/Buildings/Stabilizer.h>
#include <PDS/Player/PDSPlayerState.h>
#include <PDS/Player/MainPawn.h>
#include <PDS/Player/PDSPlayerController.h>
#include <PDS/Units/Unit.h>
#include <PDS/World/Building.h>
#include <PDS/World/Grid.h>
#include <PDS/World/GridCell.h>
#include <PDS/World/TurnManager.h>
#include <PDS/Utility/PlayerControllerRange.h>
#include <PDS/Utility/Range.h>

#include <Engine/World.h>

DEFINE_LOG_CATEGORY_STATIC(PDSGameModeBase, Display, Display);

APDSGameModeBase::APDSGameModeBase()
{
	gridClass = AGrid::StaticClass();
	DefaultPawnClass = AMainPawn::StaticClass();
	PlayerControllerClass = APDSPlayerController::StaticClass();
	PlayerStateClass = APDSPlayerState::StaticClass();
	turnManagerClass = ATurnManager::StaticClass();
}

void APDSGameModeBase::RequestTurnEnd(APDSPlayerController* player)
{
	if (turnInProgress)
		return;

	auto state = player->GetPlayerState<APDSPlayerState>();
	state->readyToFinishTurn = true;
}

void APDSGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	GenerateGrid();
	GeneratePlayerStarts();

	const auto world = GetWorld();
	turnManager = world->SpawnActor<ATurnManager>(turnManagerClass);
}

void APDSGameModeBase::GenerateGrid()
{
	const auto world = GetWorld();
	const auto grid = world->SpawnActor<AGrid>(gridClass);
	grid->SetActorLabel(TEXT("Grid0"));
	grids.Add(grid);

	const auto portalPos = FGridPos{ grid, { 0, 0 } };
	portalPos.SpawnTile(bloodStone);
	GenerateStartingPortal(portalPos);
}

void APDSGameModeBase::GenerateStartingPortal(FGridPos pos)
{
	if (portal != nullptr)
	{
		pos.Build(portal);
		const auto toStabilize = 1 + portal->GetTurnsToStabilize(pos);
		for (const auto i : Range<int32>{ 0, toStabilize })
			portal->OnTurn(pos);
	}
}

void APDSGameModeBase::GeneratePlayerStarts()
{
	const auto starts = TArray<FGridPos> {
		{ grids[0], {  0,  2 } },
		/*{ grids[0], { -2,  2 } },
		{ grids[0], {  2,  0 } },
		{ grids[0], { -2,  0 } },
		{ grids[0], {  2, -2 } },
		{ grids[0], {  0, -2 } },*/
	};
	auto id = 0;

	for (const auto player : PlayerControllerRange{ GetWorld() })
		GeneratePlayerStart(Cast<APDSPlayerController>(player), starts[id++]);
}

void APDSGameModeBase::GeneratePlayerStart(APDSPlayerController* player, FGridPos pos)
{
	const auto state = player->GetPlayerState<APDSPlayerState>();

	if (wagon != nullptr)
	{
		if (!pos.Exists())
		{
			UE_LOG(PDSGameModeBase, Error, TEXT("Wagon position doesn't exist: (%d;%d)."), pos.q(), pos.r());
			return;
		}

		const auto builtWagon = pos.Build(wagon, NAME_None, state);
		for (const auto& item : startingItems)
			builtWagon->AddItem(item);
	}

	for (const auto i : Range<int32>{0, startingUnits})
	{
		const auto unit = NewObject<UUnit>(player);
		unit->SetOwner(state);
	}

	for (const auto craft : startingCrafts)
		state->availableCrafts.Add(craft);
}
