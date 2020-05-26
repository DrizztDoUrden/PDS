#pragma once

#include <PDS/Items/Item.h>
#include <PDS/Utility/ISHelpers.h>

#include <CoreMinimal.h>
#include <GameFramework/Actor.h>
#include <HAL/Runnable.h>
#include <HAL/RunnableThread.h>

#include "TurnManager.generated.h"

class AGrid;
class APDSPlayerState;
struct FBuildStep;
struct FBuildTask;
struct FGridCell;
struct FTask;
class UBuilding;
class UUnit;

namespace Hexes
{
	struct Int;
}

UENUM(BlueprintType)
enum class ETurnSwitchSteps : uint8
{
	None,
	Buildings,
	Units,
	Tasks,
};

class FNextTurnWorker : public FRunnable
{
public:
	FORCEINLINE_DEBUGGABLE bool HasFinished() const { return finished; }
	FNextTurnWorker(UWorld* world_, int32& turn_, FTurnSwitchProgress& progress_);

	uint32 Run() override;

private:
	FTurnSwitchProgress& progress;
	int32& turn;
	bool finished = false;
	UWorld* world;
	TUniquePtr<FRunnableThread> thread;
	struct NextTurnWorkerState* threadState;

	void UpdateBuilding(TWeakObjectPtr<APlayerController> player, UBuilding* building);
	void UpdateUnit(TWeakObjectPtr<APlayerController> player, UUnit* unit);
	void UpdateTask(TWeakObjectPtr<APlayerController> player, int32 id, const FTask& task);
	void RemoveTasks(APDSPlayerState* player) const;

	void ReorderTasks(APDSPlayerState* player);
	bool ProcessBuildTask(APDSPlayerState* player, const FBuildTask& task);
	void ApplyWorkforce(APDSPlayerState* player, const FBuildStep& step, UBuilding* building);
	bool FinishBuildingStep(APDSPlayerState* player, const FBuildTask& task, UBuilding* building);

	bool RequestResources(APDSPlayerState* player, const TArray<FItem>& resources);
	int32 AllocateWorkforce(APDSPlayerState* player, int32 requested);
};

USTRUCT(BlueprintType)
struct FTurnSwitchProgress
{
	GENERATED_BODY()

public:
	FORCEINLINE_DEBUGGABLE FTurnSwitchProgress() = default;
	FORCEINLINE_DEBUGGABLE FTurnSwitchProgress(ETurnSwitchSteps step_, int32 substepCount_) : step(step_), substepCount(substepCount_) {}

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	ETurnSwitchSteps step = ETurnSwitchSteps::None;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	int32 substep = 0;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	int32 substepCount = 1;
};

UCLASS(Blueprintable, BlueprintType)
class ATurnManager : public AActor
{
	GENERATED_BODY()

public:
	ATurnManager();

	FORCEINLINE_DEBUGGABLE int32 GetTurn() const { return turn; }

private:
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	int32 turn = 1;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FTurnSwitchProgress progress;

	TUniquePtr<FNextTurnWorker> nextTurnWorker;

	void TickActor(float deltaTime, enum ELevelTick tickType, FActorTickFunction& thisTickFunction) override;
};
