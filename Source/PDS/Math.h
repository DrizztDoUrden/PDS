#pragma once

#include <PDS/World/GridPos.h>

#include <CoreMinimal.h>

struct FRawBloodField
{
public:
	static constexpr float PowConst = 1.3f;

	FORCEINLINE_DEBUGGABLE FRawBloodField(FGridPos center_, float power_)
		: center(center_), power(power_)
	{
		power -= GetTotalWaste();
	}

	FORCEINLINE_DEBUGGABLE float GetPowerAt(FGridPos pos)
	{
		const auto distance = pos.DistanceTo(center);
		return power / FMath::Pow(distance, PowConst);
	}

	FORCEINLINE_DEBUGGABLE int32 GetConsiderableRange()
	{
		return FMath::Pow(FMath::Max(0.f, power), 1 / PowConst);
	}

	FORCEINLINE_DEBUGGABLE float GetTotalWaste()
	{
		const auto range = GetConsiderableRange();
		const auto cells = (3 * range + 1) * (range + 1);
		return cells;
	}

private:
	FGridPos center;
	float power;
};
