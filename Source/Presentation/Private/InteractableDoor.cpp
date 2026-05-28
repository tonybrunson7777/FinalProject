#include "InteractableDoor.h"

#include "Components/StaticMeshComponent.h"

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
		SetActorRotation(ClosedRotation);
		bIsOpen = false;
	}
	else
	{
		ClosedRotation = GetActorRotation();
		const FRotator OpenRotation = ClosedRotation + FRotator(0.0f, OpenYawAngle, 0.0f);
		SetActorRotation(OpenRotation);
		bIsOpen = true;
	}
}
