#include <PDS/World/TurnManager.h>

#include <PDS/PDSGameModeBase.h>
#include <PDS/Player/PDSPlayerState.h>
#include <PDS/Player/PDSPlayerController.h>
#include <PDS/Units/Unit.h>
#include <PDS/World/Building.h>
#include <PDS/World/GridCell.h>
#include <PDS/WorldObjects/BuildingType.h>
#include <PDS/Utility/PlayerControllerRange.h>
#include <PDS/Hexes/Hexes.h>

#include <Algo/Accumulate.h>
#include <Algo/AllOf.h>
#include <Async/Async.h>
#include <Engine/World.h>
#include <EngineUtils.h>

DEFINE_LOG_CATEGORY_STATIC(TurnManager, Display, All);

FNextTurnWorker::FNextTurnWorker(UWorld* world_, int32& turn_, FTurnSwitchProgress& progress_)
	: progress(progress_)
	, turn(turn_)
	, world(world_)
	, thread(FRunnableThread::Create(this, TEXT("FNextTurnWorker"), 0, TPri_BelowNormal))
{
}

using PlayerPtr = TWeakObjectPtr<APlayerController>;
auto State(PlayerPtr player) { return player->GetPlayerState<APDSPlayerState>(); };

struct NextTurnWorkerState
{
	TMap<APDSPlayerState*, int32> workforceUsed = {};
	TMap<APDSPlayerState*, TArray<int32>> tasksToDelete = {};
};

uint32 FNextTurnWorker::Run()
{
	auto threadState_ = TUniquePtr<NextTurnWorkerState>{new NextTurnWorkerState()};
	threadState = threadState_.Get();

	const auto mode = world->GetAuthGameMode<APDSGameModeBase>();
	progress = {};
	mode->turnInProgress = true;
	++turn;

	const auto players = PlayerControllerRange{ world };
	auto substepOffset = 0;

	const auto fullCollectionStep = [&](ETurnSwitchSteps step, const auto& all, auto&& collectionGetter, auto&& method) {
		const auto subSteps = Algo::Accumulate(all, 0, [&](int32 v, auto c) { return v + collectionGetter(c).Num(); });
		progress = { step, subSteps };
		for (auto& group : all)
		{
			substepOffset = progress.substep;
			for (auto& item : collectionGetter(group))
			{
				method(group, item);
				++progress.substep;
			}
		}
	};

	for (auto player : players)
	{
		AsyncTask(ENamedThreads::GameThread, [=]() { Cast<APDSPlayerController>(player)->BroadcastTurnEnd(turn - 1); });
		ReorderTasks(State(player));
		threadState->workforceUsed.Add(State(player), 0);
		threadState->tasksToDelete.Add(State(player), {});
	}

	fullCollectionStep(ETurnSwitchSteps::Buildings, players,
		[&](PlayerPtr p) { return State(p)->buildings; },
		[&](PlayerPtr p, auto b) { UpdateBuilding(p, b); });

	fullCollectionStep(ETurnSwitchSteps::Units, players,
		[&](PlayerPtr p) { return State(p)->units; },
		[&](PlayerPtr p, auto u) { UpdateUnit(p, u); });


	fullCollectionStep(ETurnSwitchSteps::Tasks, players,
		[&](PlayerPtr p) { return State(p)->tasks; },
		[&](PlayerPtr p, FTask& task) { UpdateTask(p, progress.substep - substepOffset, task); });

	for (auto player : players)
		RemoveTasks(State(player));

	mode->turnInProgress = false;
	finished = true;

	for (auto player : players)
		AsyncTask(ENamedThreads::GameThread, [=]() { Cast<APDSPlayerController>(player)->BroadcastTurnStart(turn); });

	threadState = nullptr;
	return 0;
}

void FNextTurnWorker::RemoveTasks(APDSPlayerState* player) const
{
	auto& toRemove = threadState->tasksToDelete[player];
	toRemove.Sort([](auto l, auto r) { return l > r; });
	for (auto id : toRemove)
		player->tasks.RemoveAt(id, 1, false);
}

void FNextTurnWorker::UpdateBuilding(TWeakObjectPtr<APlayerController> player, UBuilding* building)
{
	building->GetType()->OnTurn(building->pos);
}

void FNextTurnWorker::UpdateUnit(TWeakObjectPtr<APlayerController> player, UUnit* unit)
{
	unit->OnTurn();
}

void FNextTurnWorker::UpdateTask(TWeakObjectPtr<APlayerController> player, int32 id, const FTask& task)
{
	const auto state = State(player);
	const auto shouldDelete = [&]() {
		switch (task.GetOrder())
		{
		case EOrders::Build: return ProcessBuildTask(state, task.AsBuild());
		default:             checkNoEntry(); return false;
		}
	}();

	if (shouldDelete)
		threadState->tasksToDelete[state].Add(id);
}

void FNextTurnWorker::ReorderTasks(APDSPlayerState* player)
{
	// Reorder tasks
}

bool FNextTurnWorker::ProcessBuildTask(APDSPlayerState* player, const FBuildTask& task)
{
	auto shouldDelete = false;
	const auto location = task.location;
	UE_LOG(TurnManager, Log, TEXT("Trying to build on hex: (%d;%d)"), location.q(), location.r());

	// if no workforce left should skip

	auto& cell = location.GetCell();
	const auto building = cell.building;
	const auto& step = building->GetStep();

	if (!building->GetType()->IsBuildable(location))
		return true;

	if (!building->resourcesApplied)
		building->resourcesApplied = RequestResources(player, step.cost);;

	if (building->resourcesApplied)
	{
		if (building->workforceApplied < step.workforce)
			ApplyWorkforce(player, step, building);

		if (building->workforceApplied >= step.workforce)
			shouldDelete = FinishBuildingStep(player, task, building);
	}

	return shouldDelete;
}

void FNextTurnWorker::ApplyWorkforce(APDSPlayerState* player, const FBuildStep& step, UBuilding* building)
{
	const auto workforceReqested = FMath::Min(
		step.workforceLimit,
		step.workforce - building->workforceApplied
	);

	const auto workforceApplied = FMath::Min(
		workforceReqested,
		AllocateWorkforce(player, workforceReqested)
	);

	building->workforceApplied += workforceApplied;
}

bool FNextTurnWorker::FinishBuildingStep(APDSPlayerState* player, const FBuildTask& task, UBuilding* building)
{
	++building->stepId;
	building->workforceApplied = 0;

	if (building->stepId != building->GetType()->builingSteps.Num())
		return false;

	const auto& pos = task.location;
	AsyncTask(ENamedThreads::GameThread, [pos]() { pos.Build(); });
	return true;
}

bool FNextTurnWorker::RequestResources(APDSPlayerState* player, const TArray<FItem>& resources)
{
	if (!Algo::AllOf(resources, [player](auto& resource) { return player->HasItems(resource); }))
		return false;

	auto resourcesCopy = resources;

	for (auto inv : player->inventories)
	{
		auto& items = inv->GetInventory();
		for (auto i = 0; i < resourcesCopy.Num(); ++i)
		{
			auto& resource = resourcesCopy[i];
			auto found = items.FindByPredicate([&resource](auto& item) { return item.type == resource.type; });

			if (!found)
				continue;

			const auto amountTaken = FMath::Min(resource.amount, found->amount);
			inv->RemoveItem({ resource.type, amountTaken });
			resource.amount -= amountTaken;

			if (resource.amount <= 0)
			{
				resourcesCopy.RemoveAt(i);
				--i;
			}
		}
	}

	if (resourcesCopy.Num() != 0)
		UE_LOG(TurnManager, Error, TEXT("FNextTurnWorker::RequestResources critical failure."));

	return true;
}

int32 FNextTurnWorker::AllocateWorkforce(APDSPlayerState* player, int32 requested)
{
	auto& used = threadState->workforceUsed[player];
	const auto available = player->totalWorkforce - used;
	const auto allocated = FMath::Min(available, requested);
	used += allocated;
	return allocated;
}

ATurnManager::ATurnManager()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = .1f;
}

void ATurnManager::TickActor(float deltaTime, enum ELevelTick tickType, FActorTickFunction& thisTickFunction)
{
	Super::TickActor(deltaTime, tickType, thisTickFunction);

	const auto world = GetWorld();
	auto everyoneReady = true;

	for (auto it = world->GetPlayerControllerIterator(); it; ++it)
	{
		if (!State(*it)->readyToFinishTurn)
		{
			everyoneReady = false;
			break;
		}
	}

	if (everyoneReady && (!nextTurnWorker.IsValid() || nextTurnWorker->HasFinished()))
	{
		for (auto it = world->GetPlayerControllerIterator(); it; ++it)
			State(*it)->readyToFinishTurn = false;
		nextTurnWorker.Reset(new FNextTurnWorker{ world, turn, progress });
	}
}
