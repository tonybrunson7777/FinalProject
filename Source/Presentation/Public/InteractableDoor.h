#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableInterface.h"
#include "InteractableDoor.generated.h"

UCLASS(Blueprintable)
class PRESENTATION_API AInteractableDoor : public AActor, public IInteractableInterface
{
	GENERATED_BODY()

public:
	AInteractableDoor();

	virtual void Interact_Implementation(AActor* Interactor) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
	TObjectPtr<class UStaticMeshComponent> DoorMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Door")
	float OpenYawAngle = 90.0f;

	/** If set, this door requires the matching virtual key from a treasure chest. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Door|Key")
	FName RequiredKeyId;

	/** When opened, removes the virtual key so the HUD can hide the key icon. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Door|Key")
	bool bConsumeKeyOnOpen = true;

	UPROPERTY(BlueprintReadOnly, Category = "Door")
	bool bIsOpen = false;

	UPROPERTY()
	FRotator ClosedRotation;

	void OpenDoor();
};
