#pragma once

#include <PDS/Items/Item.h>
#include <PDS/Items/InventoryHolder.h>
#include <PDS/WorldObjects/BuildingType.h>
#include <PDS/Utility/ISHelpers.h>

#include <CoreMinimal.h>

#include "Building.generated.h"

class APDSPlayerState;

UCLASS(BlueprintType, NotBlueprintable)
class UBuilding : public UObject, public IDynamicInventoryHolder
{
	GENERATED_BODY()

public:
	FGridPos pos;
	int32 stepId = 0;
	bool resourcesApplied = false;
	int32 workforceApplied = 0;
	int32 ageOfStabilityChange = 0;

	FORCEINLINE_DEBUGGABLE FBuildStep GetStep() const { return type->builingSteps[stepId]; };

	UFUNCTION(BlueprintCallable)
	FORCEINLINE_DEBUGGABLE bool IsMarker() const
	{
		check(instanceId != -1);
		return isMarker_;
	}

	FORCEINLINE_DEBUGGABLE void SetMarker(UBuildingType* type_, int32 instance, APDSPlayerState* owner_, FName layout)
	{
		check(type == nullptr);
		owner = owner_;
		type = type_;
		instanceId = instance;
		layoutName = layout;
		isMarker_ = true;
	}

	FORCEINLINE_DEBUGGABLE int32 SetBuilding(int32 instance, UBuildingType* type_ = nullptr)
	{
		check(type_ == nullptr || type == nullptr || type == type_);
		const auto ret = instanceId;
		instanceId = instance;
		if (type_ != nullptr)
			type = type_;
		isMarker_ = false;
		return ret;
	}

	FORCEINLINE_DEBUGGABLE APDSPlayerState* GetOwner() const { return owner; }
	FORCEINLINE_DEBUGGABLE UBuildingType* GetType() const { return type; }
	FORCEINLINE_DEBUGGABLE FName GetLayoutName() const { return layoutName; }
	FORCEINLINE_DEBUGGABLE int32 GetAge() const { return age; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE_DEBUGGABLE FBuildingLayout GetLayout() const { return type->layouts[layoutName]; }

	UFUNCTION(BlueprintCallable)
	void SetLayout(FName newLayout);

	const TArray<FItem>& GetInventory() const override { return inventory; }
	void SetOwner(APDSPlayerState* newOwner);
	void AddItem(const FItem& item) override { IDynamicInventoryHolder::AddItem(owner, inventory, item); }
	void RemoveItem(const FItem& item) override { IDynamicInventoryHolder::RemoveItem(owner, inventory, item); }
	void OnTurn() { ++age; }

private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	APDSPlayerState* owner;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UBuildingType* type;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	int32 instanceId = -1;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FName layoutName;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	int32 age;

	UPROPERTY()
	bool isMarker_;

	TArray<FItem> inventory;
};
