#include "PlayerInteractionComponent.h"

#include "BombInventoryComponent.h"
#include "InteractableInterface.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Engine/EngineTypes.h"
#include "Kismet/KismetSystemLibrary.h"

UPlayerInteractionComponent::UPlayerInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UPlayerInteractionComponent::Interact()
{
	FHitResult Hit;
	if (!FindInteractable(Hit))
	{
		// UE_LOG(LogTemp, Warning, TEXT("Interact failed: no interactable hit."));
		return false;
	}

	AActor* HitActor = Hit.GetActor();
	if (!IsValid(HitActor) || !HitActor->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass()))
	{
		// UE_LOG(LogTemp, Warning, TEXT("Interact failed: actor '%s' is not interactable."), *GetNameSafe(HitActor));
		return false;
	}

	// UE_LOG(LogTemp, Warning, TEXT("Interact success: calling Interact on '%s'."), *GetNameSafe(HitActor));
	IInteractableInterface::Execute_Interact(HitActor, GetOwner());
	return true;
}

bool UPlayerInteractionComponent::PlaceBomb()
{
	AActor* OwnerActor = GetOwner();
	if (!IsValid(OwnerActor))
	{
		// UE_LOG(LogTemp, Warning, TEXT("PlaceBomb failed: owner invalid."));
		return false;
	}

	UBombInventoryComponent* BombInventory = OwnerActor->FindComponentByClass<UBombInventoryComponent>();
	if (!IsValid(BombInventory))
	{
		// UE_LOG(LogTemp, Warning, TEXT("PlaceBomb failed: no BombInventoryComponent on '%s'."), *GetNameSafe(OwnerActor));
		return false;
	}

	const bool bPlacedBomb = BombInventory->TryPlaceBomb();
	// UE_LOG(LogTemp, Warning, TEXT("PlaceBomb result: %s"), bPlacedBomb ? TEXT("SUCCESS") : TEXT("FAILED"));
	return bPlacedBomb;
}

bool UPlayerInteractionComponent::FindInteractable(FHitResult& OutHit) const
{
	AActor* OwnerActor = GetOwner();
	UWorld* World = GetWorld();
	if (!IsValid(OwnerActor) || !IsValid(World))
	{
		// UE_LOG(LogTemp, Warning, TEXT("FindInteractable failed: owner or world invalid."));
		return false;
	}

	const FVector OwnerLocation = OwnerActor->GetActorLocation();
	const FVector OwnerForward = OwnerActor->GetActorForwardVector().GetSafeNormal2D();

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(OwnerActor);

	TArray<AActor*> NearbyActors;
	const bool bFoundActors = UKismetSystemLibrary::SphereOverlapActors(
		World,
		OwnerLocation,
		InteractionDistance,
		ObjectTypes,
		AActor::StaticClass(),
		ActorsToIgnore,
		NearbyActors
	);

	if (!bFoundActors)
	{
		// UE_LOG(LogTemp, Warning, TEXT("Interact: no actors in radius %.1f."), InteractionDistance);
		return false;
	}

	const float MaxDistanceSq = FMath::Square(InteractionDistance);
	float BestScore = TNumericLimits<float>::Max();
	AActor* BestInteractable = nullptr;

	for (AActor* Candidate : NearbyActors)
	{
		if (!IsValid(Candidate) || !Candidate->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass()))
		{
			continue;
		}

		const FVector ToCandidate = Candidate->GetActorLocation() - OwnerLocation;
		const float DistanceSq = ToCandidate.SizeSquared2D();
		if (DistanceSq > MaxDistanceSq)
		{
			continue;
		}

		const float ForwardAlignment = FVector::DotProduct(OwnerForward, ToCandidate.GetSafeNormal2D());
		const float Score = DistanceSq - (ForwardAlignment * InteractionDistance * InteractionDistance);

		if (Score < BestScore)
		{
			BestScore = Score;
			BestInteractable = Candidate;
		}
	}

	if (!IsValid(BestInteractable))
	{
		// UE_LOG(LogTemp, Warning, TEXT("Interact: no interactable in radius %.1f."), InteractionDistance);
		return false;
	}

	OutHit = FHitResult(BestInteractable, nullptr, BestInteractable->GetActorLocation(), FVector::ZeroVector);
	// UE_LOG(LogTemp, Warning, TEXT("Interact: selected '%s'."), *GetNameSafe(BestInteractable));
	return true;
}
