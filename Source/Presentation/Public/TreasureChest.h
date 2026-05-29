#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableInterface.h"
#include "TreasureChest.generated.h"

UCLASS(Blueprintable)
class PRESENTATION_API ATreasureChest : public AActor, public IInteractableInterface
{
	GENERATED_BODY()

public:
	ATreasureChest();

	virtual void Interact_Implementation(AActor* Interactor) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Chest")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Chest")
	TObjectPtr<class UStaticMeshComponent> ChestMesh;

	/** Shared ID with the door this chest unlocks (e.g. DungeonKey1). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Chest")
	FName KeyId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Chest")
	bool bDestroyOnOpen = true;

	UPROPERTY(BlueprintReadOnly, Category = "Chest")
	bool bHasBeenOpened = false;
};
