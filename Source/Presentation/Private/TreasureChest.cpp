#include "TreasureChest.h"

#include "Components/StaticMeshComponent.h"
#include "VirtualKeyInventoryComponent.h"

ATreasureChest::ATreasureChest()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	ChestMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChestMesh"));
	ChestMesh->SetupAttachment(Root);
	ChestMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ChestMesh->SetCollisionObjectType(ECC_WorldDynamic);
	ChestMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	ChestMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

void ATreasureChest::Interact_Implementation(AActor* Interactor)
{
	if (bHasBeenOpened || !IsValid(Interactor) || KeyId.IsNone())
	{
		return;
	}

	UVirtualKeyInventoryComponent* KeyInventory = Interactor->FindComponentByClass<UVirtualKeyInventoryComponent>();
	if (!IsValid(KeyInventory) || !KeyInventory->AcquireVirtualKey(KeyId))
	{
		return;
	}

	bHasBeenOpened = true;

	if (bDestroyOnOpen)
	{
		Destroy();
	}
	else
	{
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
	}
}
