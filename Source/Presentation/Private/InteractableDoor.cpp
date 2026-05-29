#include "InteractableDoor.h"

#include "Components/StaticMeshComponent.h"
#include "VirtualKeyInventoryComponent.h"

AInteractableDoor::AInteractableDoor()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorMesh->SetupAttachment(Root);
	DoorMesh->SetCollisionProfileName(TEXT("BlockAll"));
}

void AInteractableDoor::Interact_Implementation(AActor* Interactor)
{
	if (bIsOpen)
	{
		return;
	}

	if (!RequiredKeyId.IsNone())
	{
		UVirtualKeyInventoryComponent* KeyInventory = IsValid(Interactor)
			? Interactor->FindComponentByClass<UVirtualKeyInventoryComponent>()
			: nullptr;

		if (!IsValid(KeyInventory) || !KeyInventory->HasVirtualKey(RequiredKeyId))
		{
			return;
		}

		OpenDoor();

		if (bConsumeKeyOnOpen)
		{
			KeyInventory->ConsumeVirtualKey(RequiredKeyId);
		}

		return;
	}

	OpenDoor();
}

void AInteractableDoor::OpenDoor()
{
	if (bIsOpen)
	{
		return;
	}

	ClosedRotation = GetActorRotation();
	const FRotator OpenRotation = ClosedRotation + FRotator(0.0f, OpenYawAngle, 0.0f);
	SetActorRotation(OpenRotation);
	bIsOpen = true;
}
