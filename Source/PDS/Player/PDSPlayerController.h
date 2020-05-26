#pragma once

#include <PDS/Items/Item.h>
#include <PDS/World/GridPos.h>
#include <PDS/Hexes/Hexes.h>
#include <PDS/Player/Task.h>
#include <PDS/Utility/ISHelpers.h>
#include <PDS/Utility/Threads.h>

#include <CoreMinimal.h>
#include <GameFramework/PlayerController.h>

#include "PDSPlayerController.generated.h"

class UBuildingType;

UENUM(BlueprintType)
enum class ESelectionType : uint8
{
	None,
	Building,
	Hex,
};

UENUM(BlueprintType)
enum class EOrderTargetingType : uint8
{
	None,
	EmptyHex,
};

FORCEINLINE_DEBUGGABLE EOrderTargetingType GetOrderTargetingType(EOrders order)
{
	switch (order)
	{
	case EOrders::Build:
		return EOrderTargetingType::EmptyHex;
	case EOrders::None:
		return EOrderTargetingType::None;
	default:
		checkNoEntry();
		return EOrderTargetingType::None;
	}
}

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTurnSwitch, int32, turnNumber);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FIntenoryItemUpdate, const FItem&, newValue);

UCLASS(BlueprintType, Blueprintable)
class APDSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FTurnSwitch onTurnStart;

	UPROPERTY(BlueprintAssignable)
	FTurnSwitch onTurnEnd;

	UPROPERTY(BlueprintAssignable)
	FIntenoryItemUpdate onItemAdded;

	UPROPERTY(BlueprintAssignable)
	FIntenoryItemUpdate onItemRemoved;

	UPROPERTY(BlueprintAssignable)
	FIntenoryItemUpdate onItemChanged;

	APDSPlayerController();

	UFUNCTION(BlueprintCallable)
	void ResetSelection() { selection.type = ESelectionType::None; }

	FORCEINLINE_DEBUGGABLE void SelectHex(FGridPos hex)
	{
		const auto alreadySelectedBuilding = selection.type == ESelectionType::Building && selection.hex == hex;
		const auto shouldSelectBuilding = hex.HasBuilding() && !alreadySelectedBuilding;
		selection.type = shouldSelectBuilding ? ESelectionType::Building : ESelectionType::Hex;
		selection.hex = hex;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE_DEBUGGABLE ESelectionType GetSelectionType() const { return selection.type; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE_DEBUGGABLE EOrders GetPartialOrder() const { return partialOrder.type; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE_DEBUGGABLE FGridPos GetSelectedHex() const
	{
		check(selection.type == ESelectionType::Building || selection.type == ESelectionType::Hex);
		return selection.hex;
	}

	UFUNCTION(BlueprintCallable)
	void InitiateBuildOrder(UBuildingType* buildingType, FName layout);

	UFUNCTION(BlueprintCallable)
	void CancelOrder();

	UFUNCTION(Server, Unreliable)
	void FinishOrder(FGridPos target);
	void FinishOrder_Implementation(FGridPos target);

	bool ValidateOrder(FGridPos target) const;

	UFUNCTION(BlueprintCallable, Server, Unreliable)
	void FinishTurn();
	void FinishTurn_Implementation();

	FORCEINLINE_DEBUGGABLE void BroadcastTurnEnd(int32 turn) { ExecuteInGameThread([=]() { BroadcastTurnEndWorker(turn); }); }
	FORCEINLINE_DEBUGGABLE void BroadcastTurnStart(int32 turn) { ExecuteInGameThread([=]() { BroadcastTurnStartWorker(turn); }); }

	FORCEINLINE_DEBUGGABLE void BroadcastItemAdded(const FItem& newValue) { ExecuteInGameThread([=]() { BroadcastItemAddedWorker(newValue); }); }
	FORCEINLINE_DEBUGGABLE void BroadcastItemRemoved(const FItem& newValue) { ExecuteInGameThread([=]() { BroadcastItemRemovedWorker(newValue); }); }
	FORCEINLINE_DEBUGGABLE void BroadcastItemChanged(const FItem& newValue) { ExecuteInGameThread([=]() { BroadcastItemChangedWorker(newValue); }); }

	TOptional<FGridPos> GetPosUnderCursor() const;

private:
	struct
	{
		ESelectionType type = ESelectionType::None;
		FGridPos hex;
	} selection;

	struct
	{
		EOrders type = EOrders::None;
		struct
		{
			UBuildingType* buildingType = nullptr;
			FName layout = NAME_None;
		} build;
	} partialOrder;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Building Cursor", meta = (AllowPrivateAccess=true))
	TSubclassOf<AActor> buildingCursorClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Building Cursor", meta = (AllowPrivateAccess=true))
	UMaterialInterface* buildingCursorMaterial;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Building Cursor", meta = (AllowPrivateAccess=true))
	UMaterialInterface* buildingCursorErrorMaterial;

	struct {
		FGridPos pos;
		AActor* actor;
		bool wasError = false;
	} buildingCursor;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Select Hex"))
	void SelectHexBP(FGridPos hex) { SelectHex(hex); }

	UFUNCTION(NetMulticast, Unreliable)
	void BroadcastTurnStartWorker(int32 turn);
	FORCEINLINE_DEBUGGABLE void BroadcastTurnStartWorker_Implementation(int32 turn) { onTurnStart.Broadcast(turn); }

	UFUNCTION(NetMulticast, Unreliable)
	void BroadcastTurnEndWorker(int32 turn);
	FORCEINLINE_DEBUGGABLE void BroadcastTurnEndWorker_Implementation(int32 turn) { onTurnEnd.Broadcast(turn); }

	UFUNCTION(NetMulticast, Unreliable)
	void BroadcastItemAddedWorker(const FItem& newValue);
	FORCEINLINE_DEBUGGABLE void BroadcastItemAddedWorker_Implementation(const FItem& newValue) { onItemAdded.Broadcast(newValue); }

	UFUNCTION(NetMulticast, Unreliable)
	void BroadcastItemRemovedWorker(const FItem& newValue);
	FORCEINLINE_DEBUGGABLE void BroadcastItemRemovedWorker_Implementation(const FItem& newValue) { onItemRemoved.Broadcast(newValue); }

	UFUNCTION(NetMulticast, Unreliable)
	void BroadcastItemChangedWorker(const FItem& newValue);
	FORCEINLINE_DEBUGGABLE void BroadcastItemChangedWorker_Implementation(const FItem& newValue) { onItemChanged.Broadcast(newValue); }

	void BeginPlay() override;
	void TickActor(float deltaTime, enum ELevelTick tickType, FActorTickFunction& thisTickFunction) override;
	virtual void UpdateBuildingCursor();
	virtual void SwitchBuildingCursorToValid();
	virtual void SwitchBuildingCursorToInvalid();
};
