#pragma once

#include <PDS/World/GridPos.h>
#include <PDS/WorldObjects/BuildingType.h>
#include <PDS/Utility/ISHelpers.h>

#include <CoreMinimal.h>

#include "Stabilizer.generated.h"

class UTileType;

struct FStabilisationResult
{
public:
	Hexes::Int pos;
	int32 stability;
	bool bloodStone;
};

USTRUCT(BlueprintType)
struct FStabilityRange
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stabilizer")
	int32 start = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stabilizer")
	int32 bonus = 1;
};

USTRUCT(BlueprintType)
struct FStabilizationTier
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stabilizer")
	int32 requiredStability = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stabilizer")
	int32 turnsToStabilize = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stabilizer")
	int32 stabilizationRange = 7;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stabilizer")
	int32 bloodStoneRange = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stabilizer")
	TArray<FStabilityRange> stabilityProvided{ FStabilityRange{}, };

	int32 GetStabilityBonus(int32 range) const;
};

UCLASS(Blueprintable, BlueprintType)
class UStabilizer : public UBuildingType
{
	GENERATED_BODY()

public:
	FORCEINLINE_DEBUGGABLE int32 GetTurnsToStabilize(FGridPos hex) const { return GetTier(hex).turnsToStabilize; }
	FORCEINLINE_DEBUGGABLE const FStabilizationTier& GetTier(FGridPos hex) const { return stabilization[GetTier(hex.GetStability())]; }
	int32 GetTurnsSinceStabilizationStart(FGridPos hex) const;

	void OnTurn(FGridPos hex) const override;
	void OnStabilityChange(FGridPos hex, int32 oldStability) const override;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stabilizer", meta = (AllowPrivateAccess = true))
	TArray<FStabilizationTier> stabilization { FStabilizationTier{}, };

	UPROPERTY(EditDefaultsOnly, Category = "Tiles")
	UTileType* bloodStone;

	UPROPERTY(EditDefaultsOnly, Category = "Tiles")
	UTileType* grass;

	UPROPERTY(EditDefaultsOnly, Category = "Tiles")
	UTileType* water;

	virtual void Stabilize(FGridPos hex) const;
	virtual TArray<FStabilisationResult> ProduceStabilityBonuses(FGridPos hex) const;
	virtual void OnTierSwitch(FGridPos hex) const {}
	virtual void OnTierStabilized(FGridPos hex) const {}

	int32 GetTier(int32 stability) const;
};
