#include "HeartPickup.h"

#include "Components/StaticMeshComponent.h"
#include "HeartHealthComponent.h"
#include "GameFramework/Actor.h"

AHeartPickup::AHeartPickup()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	HeartMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HeartMesh"));
	HeartMesh->SetupAttachment(Root);
	HeartMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	HeartMesh->SetCollisionObjectType(ECC_WorldDynamic);
	HeartMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	HeartMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

void AHeartPickup::Interact_Implementation(AActor* Interactor)
{
	if (!IsValid(Interactor))
	{
		return;
	}

	UHeartHealthComponent* HeartHealth = Interactor->FindComponentByClass<UHeartHealthComponent>();
	if (!IsValid(HeartHealth) || HeartHealth->IsDead())
	{
		return;
	}

	if (HeartHealth->CurrentHearts >= HeartHealth->MaxHearts)
	{
		return;
	}

	HeartHealth->HealHearts(FMath::Max(1, HeartsToRestore));

	if (bDestroyOnPickup)
	{
		Destroy();
	}
	else
	{
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
	}
}
