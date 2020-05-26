#include <PDS/Buildings/Stabilizer.h>

#include <PDS/World/Building.h>
#include <PDS/World/GridCell.h>
#include <PDS/Hexes/Shapes/Spiral.h>
#include <PDS/Utility/Threads.h>
#include <PDS/Utility/Range.h>

#include <Algo/AnyOf.h>

DEFINE_LOG_CATEGORY_STATIC(Stabilizer, Display, All)

int32 FStabilizationTier::GetStabilityBonus(int32 range) const
{
	if (range > stabilizationRange)
		return 0;
	for (const auto i : Range<int32>{ stabilityProvided })
		if (stabilityProvided[i].start <= range && (i == stabilityProvided.Num() - 1 || stabilityProvided[i + 1].start > range))
			return stabilityProvided[i].bonus;
	checkNoEntry();
	return -1;
}

int32 UStabilizer::GetTurnsSinceStabilizationStart(FGridPos hex) const
{
	const auto building = hex.GetBuilding();
	return building->GetAge() - building->ageOfStabilityChange;
}

void UStabilizer::OnTurn(FGridPos hex) const
{
	Super::OnTurn(hex);

	const auto building = hex.GetBuilding();
	const auto age = GetTurnsSinceStabilizationStart(hex);
	const auto tier = GetTier(hex.GetStability());

	if (tier > -1 && age > 0 && age <= stabilization[tier].turnsToStabilize)
		Stabilize(hex);
}

void UStabilizer::OnStabilityChange(FGridPos hex, int32 oldStability) const
{
	Super::OnStabilityChange(hex, oldStability);

	const auto building = hex.GetBuilding();
	const auto stability = hex.GetStability();
	const auto oldTier = GetTier(oldStability);
	const auto tier = GetTier(hex.GetStability());

	if (tier > oldTier)
	{
		building->ageOfStabilityChange = hex.GetBuilding()->GetAge();
		OnTierSwitch(hex);
	}
}

void UStabilizer::Stabilize(FGridPos hex) const
{
	const auto building = hex.GetBuilding();

	if (!bloodStone || !water || !grass)
	{
		UE_LOG(Stabilizer, Display, TEXT("One of %s tile type properties not set."), *GetClass()->GetName());
		return;
	}

	const auto area = ProduceStabilityBonuses(hex);
	for (auto& effect : area)
	{
		const auto areaPos = FGridPos{ hex.GetGrid(), effect.pos };

		if (effect.bloodStone)
		{
			ExecuteInGameThread([this, areaPos, hex, effect]() {
				areaPos.SpawnTile(bloodStone);
				areaPos.SetStability(hex, effect.stability);
			});
		}
		else
		{
			static const auto rand = FRandomStream{};
			const auto tileType = rand.GetFraction() < .1f ? water : grass;
			ExecuteInGameThread([areaPos, tileType, hex, effect]() {
				if (!areaPos.Exists())
					areaPos.SpawnTile(tileType);
				areaPos.SetStability(hex, effect.stability);
			});
		}
	}

	if (GetTurnsSinceStabilizationStart(hex) == GetTier(hex).turnsToStabilize)
		OnTierStabilized(hex);
}

TArray<FStabilisationResult> UStabilizer::ProduceStabilityBonuses(FGridPos hex) const
{
	const auto tierId = GetTier(hex.GetStability());

	if (tierId == -1)
		return {};

	const auto tier = stabilization[tierId];
	const auto building = hex.GetBuilding();
	const auto age = GetTurnsSinceStabilizationStart(hex);
	const auto center = hex.GetHex();
	const auto bsStart = FMath::Max(2, 1 + FMath::FloorToInt(static_cast<float>(tier.bloodStoneRange) * (age - 1) / tier.turnsToStabilize));
	const auto bsEnd = FMath::FloorToInt(static_cast<float>(tier.bloodStoneRange) * age / tier.turnsToStabilize);
	auto ret = TArray<FStabilisationResult>{};

	if (age == 1)
		ret.Add({ center, tier.GetStabilityBonus(0), true });
	for (const auto& cHex : Hexes::MakeSpiral(bsEnd, center, bsStart))
		ret.Add({ cHex, tier.GetStabilityBonus(hex.DistanceTo(cHex)), true });
	if (age == tier.turnsToStabilize)
		for (const auto& cHex : Hexes::MakeSpiral(tier.stabilizationRange, center, tier.bloodStoneRange + 1))
			ret.Add({ cHex, tier.GetStabilityBonus(hex.DistanceTo(cHex)), false });

	return ret;
}

int32 UStabilizer::GetTier(int32 stability) const
{
	for (const auto& tier : Range<int32>{ stabilization })
		if (stabilization[tier].requiredStability <= stability && (tier == stabilization.Num() - 1 || stabilization[tier + 1].requiredStability > stability))
			return tier;
	return -1;
}
