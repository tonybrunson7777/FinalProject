#include "PlayerInteractionComponent.h"

#include "BombInventoryComponent.h"
#include "InteractableInterface.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"

UPlayerInteractionComponent::UPlayerInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UPlayerInteractionComponent::Interact()
{
	FHitResult Hit;
	if (!FindInteractable(Hit))
	{
		UE_LOG(LogTemp, Warning, TEXT("Interact failed: no interactable hit."));
		return false;
	}

	AActor* HitActor = Hit.GetActor();
	if (!IsValid(HitActor) || !HitActor->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Interact failed: actor '%s' is not interactable."), *GetNameSafe(HitActor));
		return false;
	}

	UE_LOG(LogTemp, Warning, TEXT("Interact success: calling Interact on '%s'."), *GetNameSafe(HitActor));
	IInteractableInterface::Execute_Interact(HitActor, GetOwner());
	return true;
}

bool UPlayerInteractionComponent::PlaceBomb()
{
	AActor* OwnerActor = GetOwner();
	if (!IsValid(OwnerActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("PlaceBomb failed: owner invalid."));
		return false;
	}

	UBombInventoryComponent* BombInventory = OwnerActor->FindComponentByClass<UBombInventoryComponent>();
	if (!IsValid(BombInventory))
	{
		UE_LOG(LogTemp, Warning, TEXT("PlaceBomb failed: no BombInventoryComponent on '%s'."), *GetNameSafe(OwnerActor));
		return false;
	}

	const bool bPlacedBomb = BombInventory->TryPlaceBomb();
	UE_LOG(LogTemp, Warning, TEXT("PlaceBomb result: %s"), bPlacedBomb ? TEXT("SUCCESS") : TEXT("FAILED"));
	return bPlacedBomb;
}

bool UPlayerInteractionComponent::FindInteractable(FHitResult& OutHit) const
{
	AActor* OwnerActor = GetOwner();
	UWorld* World = GetWorld();
	if (!IsValid(OwnerActor) || !IsValid(World))
	{
		UE_LOG(LogTemp, Warning, TEXT("FindInteractable failed: owner or world invalid."));
		return false;
	}

	const FVector Start = OwnerActor->GetActorLocation() + FVector(0.0f, 0.0f, 50.0f);
	const FVector End = Start + OwnerActor->GetActorForwardVector() * InteractionDistance;

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(PlayerInteractTrace), false, OwnerActor);
	const bool bHit = World->SweepSingleByChannel(
		OutHit,
		Start,
		End,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeSphere(InteractionRadius),
		QueryParams
	);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Interact trace: hit=%s actor='%s' start=%s end=%s"),
		bHit ? TEXT("TRUE") : TEXT("FALSE"),
		*GetNameSafe(OutHit.GetActor()),
		*Start.ToCompactString(),
		*End.ToCompactString()
	);

	if (bHit && IsValid(OutHit.GetActor()) && OutHit.GetActor()->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass()))
	{
		return true;
	}

	// Top-down fallback: sweep a larger sphere around the player and pick the nearest interactable.
	TArray<FHitResult> FallbackHits;
	FCollisionQueryParams FallbackParams(SCENE_QUERY_STAT(PlayerInteractFallbackSweep), false, OwnerActor);
	const bool bFoundFallbackHits = World->SweepMultiByChannel(
		FallbackHits,
		Start,
		Start,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeSphere(InteractionDistance),
		FallbackParams
	);

	float ClosestDistanceSq = TNumericLimits<float>::Max();
	AActor* ClosestInteractable = nullptr;

	if (bFoundFallbackHits)
	{
		for (const FHitResult& FallbackHit : FallbackHits)
		{
			AActor* Candidate = FallbackHit.GetActor();
			if (!IsValid(Candidate) || !Candidate->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass()))
			{
				continue;
			}

			const float DistSq = FVector::DistSquared(OwnerActor->GetActorLocation(), Candidate->GetActorLocation());
			if (DistSq < ClosestDistanceSq)
			{
				ClosestDistanceSq = DistSq;
				ClosestInteractable = Candidate;
			}
		}
	}

	if (IsValid(ClosestInteractable))
	{
		OutHit = FHitResult(ClosestInteractable, nullptr, ClosestInteractable->GetActorLocation(), FVector::ZeroVector);
		UE_LOG(LogTemp, Warning, TEXT("Interact overlap fallback: selected '%s'."), *GetNameSafe(ClosestInteractable));
		return true;
	}

	UE_LOG(LogTemp, Warning, TEXT("Interact overlap fallback: no interactable in radius %.1f."), InteractionDistance);
	return false;
}
