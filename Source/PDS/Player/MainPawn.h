#pragma once

#include <CoreMinimal.h>
#include <GameFramework/DefaultPawn.h>

#include "MainPawn.generated.h"

using namespace UC;
using namespace UP;
using namespace UF;
using namespace UM;

class UFloatingPawnMovement;

UCLASS(Blueprintable, BlueprintType)
class AMainPawn : public ADefaultPawn
{
	GENERATED_BODY()

public:
	AMainPawn();

private:
	void BeginPlay() override;
	void SetupPlayerInputComponent(UInputComponent* playerInputComponent) override;

	UFUNCTION()
	void SelectOrAct();

	UFUNCTION()
	void CancelOrder();

	UFUNCTION()
	void ReadyForNextTurn();

	UFUNCTION()
	void MoveNorth(float delta);

	UFUNCTION()
	void MoveEast(float delta);

	UFUNCTION()
	void MoveUp();

	UFUNCTION()
	void MoveDown();
};
