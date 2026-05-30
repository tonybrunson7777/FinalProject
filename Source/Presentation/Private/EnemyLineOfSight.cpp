#include "EnemyLineOfSight.h"

#include "Engine/World.h"
#include "GameFramework/Actor.h"

namespace PresentationEnemyVisibility
{
	bool HasLineOfSightTo(const AActor* Observer, const AActor* Target, float ObserverEyeHeight, float TargetEyeHeight)
	{
		if (!IsValid(Observer) || !IsValid(Target))
		{
			return false;
		}

		const UWorld* World = Observer->GetWorld();
		if (!IsValid(World))
		{
			return false;
		}

		const FVector TraceStart = Observer->GetActorLocation() + FVector(0.0f, 0.0f, ObserverEyeHeight);
		const FVector TraceEnd = Target->GetActorLocation() + FVector(0.0f, 0.0f, TargetEyeHeight);

		FCollisionObjectQueryParams ObjectQueryParams;
		ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
		ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);

		FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(EnemyLineOfSight), false, Observer);
		QueryParams.AddIgnoredActor(Observer);
		QueryParams.AddIgnoredActor(Target);

		FHitResult Hit;
		const bool bBlockingHit = World->LineTraceSingleByObjectType(
			Hit,
			TraceStart,
			TraceEnd,
			ObjectQueryParams,
			QueryParams
		);

		// Clear line of sight when no world geometry blocks the path (ignores pawns/triggers).
		return !bBlockingHit;
	}
}
