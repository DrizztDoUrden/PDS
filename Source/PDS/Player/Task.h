#pragma once

#include <PDS/Player/Orders.h>
#include <PDS/World/GridPos.h>

#include <CoreMinimal.h>

#include "Task.generated.h"

using namespace US;
using namespace UP;
using namespace UM;

class AGrid;
class UBuildingType;
class UCraftDefinition;

struct FTaskImpl
{
protected:
	FORCEINLINE_DEBUGGABLE FTaskImpl() = default;
	FORCEINLINE_DEBUGGABLE FTaskImpl(const FTaskImpl&) = default;
	FORCEINLINE_DEBUGGABLE FTaskImpl(FTaskImpl&&) = default;
};

struct FBuildTask : public FTaskImpl
{
	FGridPos location;

	FORCEINLINE_DEBUGGABLE FBuildTask(FGridPos location_) : location(location_) {}
};

struct FRecurringTask : public FTaskImpl
{
	bool isRecurring = false;
};

struct FCraftTask : public FRecurringTask
{
	FGridPos building;
	UCraftDefinition* recipe;
	int32 workforceRequested;

	FORCEINLINE_DEBUGGABLE FCraftTask(FGridPos building_, UCraftDefinition* recipe_, int32 workforceRequested_)
		: building(building_)
		, recipe(recipe_)
		, workforceRequested(workforceRequested_)
	{}
};

struct FHarvestTask : public FRecurringTask
{

};

USTRUCT(BlueprintType)
struct FTask
{
	GENERATED_BODY()

public:
	FORCEINLINE_DEBUGGABLE EOrders GetOrder() const { return order; }

	FORCEINLINE_DEBUGGABLE FTask() = default;
	FORCEINLINE_DEBUGGABLE FTask(const FTask& other) : order(other.order), impl(CopyImpl(other)) {}
	FORCEINLINE_DEBUGGABLE FTask(FBuildTask&& parameters) : order(EOrders::Build), impl(MakeUnique<FBuildTask>(parameters)) {}
	FORCEINLINE_DEBUGGABLE FTask(FCraftTask&& parameters) : order(EOrders::Craft), impl(MakeUnique<FCraftTask>(parameters)) {}

	FORCEINLINE_DEBUGGABLE FTask& operator=(const FTask& other)
	{
		order = other.order;
		impl = CopyImpl(other);
	}

	FORCEINLINE_DEBUGGABLE const FBuildTask& AsBuild() const { check(order == EOrders::Build); return static_cast<const FBuildTask&>(*impl); }
	FORCEINLINE_DEBUGGABLE const FCraftTask& AsCraft() const { check(order == EOrders::Craft); return static_cast<const FCraftTask&>(*impl); }

private:
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess = true))
	EOrders order = EOrders::None;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	int32 priority = 100;

	TUniquePtr<FTaskImpl> impl;

	FORCEINLINE_DEBUGGABLE static TUniquePtr<FTaskImpl> CopyImpl(const FTask& source)
	{
		switch (source.order)
		{
		case EOrders::None: return {};
		case EOrders::Build: return MakeUnique<FBuildTask>(source.AsBuild());
		case EOrders::Craft: return MakeUnique<FCraftTask>(source.AsCraft());
		default:
			checkNoEntry();
			return {};
		}
	}
};
