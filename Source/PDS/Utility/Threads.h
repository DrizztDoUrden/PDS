#pragma once

#include <CoreMinimal.h>

void ExecuteInGameThread(TUniqueFunction<void()>&& delegate);
