#include <PDS/Units/Unit.h>

#include <PDS/Player/PDSPlayerState.h>

void UUnit::AddToPlayer()
{
	owner->units.Add(this);
	owner->totalWorkforce += WorkforceProvided();
}

void UUnit::RemoveFromPlayer()
{
	owner->units.Remove(this);
	owner->totalWorkforce -= WorkforceProvided();
}
