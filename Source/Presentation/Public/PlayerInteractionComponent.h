#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerInteractionComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PRESENTATION_API UPlayerInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerInteractionComponent();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	float InteractionDistance = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	float InteractionRadius = 24.0f;

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	bool Interact();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	bool PlaceBomb();

protected:
	bool FindInteractable(FHitResult& OutHit) const;
};
