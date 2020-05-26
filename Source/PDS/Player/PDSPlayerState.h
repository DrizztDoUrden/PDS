#pragma once

#include <PDS/Items/InventoryHolder.h>
#include <PDS/Items/Item.h>
#include <PDS/Player/Task.h>
#include <PDS/Utility/ISHelpers.h>

#include <CoreMinimal.h>
#include <GameFramework/PlayerState.h>

#include "PDSPlayerState.generated.h"

class UBuilding;
class UCraftDefinition;
class UUnit;

UCLASS(BlueprintType, Blueprintable)
class APDSPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	TArray<FTask> tasks;

	UPROPERTY(BlueprintReadOnly)
	TMap<FName, FItem> combinedInventory;

	UPROPERTY(BlueprintReadOnly)
	TArray<TScriptInterface<IDynamicInventoryHolder>> inventories;

	UPROPERTY(BlueprintReadOnly)
	TArray<UUnit*> units;

	UPROPERTY(BlueprintReadOnly)
	TArray<UCraftDefinition*> availableCrafts;

	TArray<UBuilding*> buildings;

	int32 totalWorkforce = 0;

	UPROPERTY(BlueprintReadWrite)
	bool readyToFinishTurn = false;

	UPROPERTY(BlueprintReadOnly, Replicated)
	int32 inventoryCapacity;

	UPROPERTY(BlueprintReadOnly, Replicated)
	int32 livingCapacity;

	UPROPERTY(BlueprintReadOnly, Replicated)
	int32 workshopCapacity;

	void RegisterItem(const FItem& item);
	void UnregisterItem(const FItem& item);

	bool HasItems(const FItem& item) const;

private:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
