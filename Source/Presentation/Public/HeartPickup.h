#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableInterface.h"
#include "HeartPickup.generated.h"

UCLASS(Blueprintable)
class PRESENTATION_API AHeartPickup : public AActor, public IInteractableInterface
{
	GENERATED_BODY()

public:
	AHeartPickup();

	virtual void Interact_Implementation(AActor* Interactor) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Heart")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Heart")
	TObjectPtr<class UStaticMeshComponent> HeartMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Heart", meta = (ClampMin = "1", UIMin = "1"))
	int32 HeartsToRestore = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Heart")
	bool bDestroyOnPickup = true;
};
