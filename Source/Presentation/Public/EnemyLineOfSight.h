#pragma once

#include "CoreMinimal.h"

class AActor;

namespace PresentationEnemyVisibility
{
	/** Returns false if world geometry blocks the line between observer and target. */
	PRESENTATION_API bool HasLineOfSightTo(
		const AActor* Observer,
		const AActor* Target,
		float ObserverEyeHeight = 50.0f,
		float TargetEyeHeight = 50.0f
	);
}
