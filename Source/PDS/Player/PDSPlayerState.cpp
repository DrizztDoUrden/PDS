#include <PDS/Player/PDSPlayerState.h>

#include <PDS/Player/PDSPlayerController.h>

#include <Engine/World.h>
#include <GameFramework/PlayerController.h>
#include <Net/UnrealNetwork.h>

static APDSPlayerController* GetPlayerController(UWorld* world, APDSPlayerState* playerState)
{
	for (auto it = world->GetPlayerControllerIterator(); it; ++it)
		if ((*it)->GetPlayerState<APDSPlayerState>() == playerState)
			return Cast<APDSPlayerController>(*it);

	return nullptr;
}

void APDSPlayerState::RegisterItem(const FItem& item)
{
	const auto existingItem = combinedInventory.Find(item.type->name);

	if (existingItem)
	{
		existingItem->amount += item.amount;

		GetPlayerController(GetWorld(), this)->BroadcastItemChanged(*existingItem);
		return;
	}

	combinedInventory.Add(item.type->name, item);
	GetPlayerController(GetWorld(), this)->BroadcastItemAdded(item);
}

void APDSPlayerState::UnregisterItem(const FItem& item)
{
	const auto existingItem = combinedInventory.Find(item.type->name);

	if (!existingItem)
		return;

	existingItem->amount -= item.amount;
	if (existingItem->amount <= 0)
	{
		combinedInventory.Remove(item.type->name);
		GetPlayerController(GetWorld(), this)->BroadcastItemRemoved({item.type, 0});
	}
	else
		GetPlayerController(GetWorld(), this)->BroadcastItemChanged(*existingItem);
}

void APDSPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APDSPlayerState, inventoryCapacity);
	DOREPLIFETIME(APDSPlayerState, livingCapacity);
	DOREPLIFETIME(APDSPlayerState, workshopCapacity);
}

bool APDSPlayerState::HasItems(const FItem& item) const
{
	const auto found = combinedInventory.Find(item.type->name);
	return found && found->amount >= item.amount;
}