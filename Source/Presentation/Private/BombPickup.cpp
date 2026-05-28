#include "BombPickup.h"

#include "BombInventoryComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Actor.h"

ABombPickup::ABombPickup()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	BombMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BombMesh"));
	BombMesh->SetupAttachment(Root);
	BombMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BombMesh->SetCollisionObjectType(ECC_WorldDynamic);
	BombMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	// Interaction trace uses Visibility; block it so pickup is always detectable.
	BombMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

void ABombPickup::Interact_Implementation(AActor* Interactor)
{
	if (!IsValid(Interactor))
	{
		UE_LOG(LogTemp, Warning, TEXT("BombPickup Interact failed: interactor invalid."));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("BombPickup Interact called by '%s' on pickup '%s'."), *GetNameSafe(Interactor), *GetNameSafe(this));

	UBombInventoryComponent* BombInventory = Interactor->FindComponentByClass<UBombInventoryComponent>();
	if (!IsValid(BombInventory))
	{
		UE_LOG(LogTemp, Warning, TEXT("BombPickup failed: '%s' has no BombInventoryComponent."), *GetNameSafe(Interactor));
		return;
	}

	if (!*BombClassToGive)
	{
		UE_LOG(LogTemp, Warning, TEXT("BombPickup failed: BombClassToGive is not set on '%s'."), *GetNameSafe(this));
		return;
	}

	BombInventory->AddBomb(BombClassToGive, FMath::Max(1, BombsGranted));
	UE_LOG(LogTemp, Warning, TEXT("BombPickup success: granted %d bomb(s) of class '%s'."), FMath::Max(1, BombsGranted), *GetNameSafe(BombClassToGive.Get()));

	if (bDestroyOnPickup)
	{
		UE_LOG(LogTemp, Warning, TEXT("BombPickup '%s' destroying after pickup."), *GetNameSafe(this));
		Destroy();
	}
}
