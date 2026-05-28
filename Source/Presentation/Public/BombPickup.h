#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableInterface.h"
#include "BombPickup.generated.h"

UCLASS(Blueprintable)
class PRESENTATION_API ABombPickup : public AActor, public IInteractableInterface
{
	GENERATED_BODY()

public:
	ABombPickup();

	virtual void Interact_Implementation(AActor* Interactor) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bomb")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bomb")
	TObjectPtr<class UStaticMeshComponent> BombMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bomb")
	TSubclassOf<AActor> BombClassToGive;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bomb")
	int32 BombsGranted = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bomb")
	bool bDestroyOnPickup = true;
};
