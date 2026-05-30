#include "CrackedWall.h"

#include "Components/StaticMeshComponent.h"
#include "HeartHealthComponent.h"

ACrackedWall::ACrackedWall()
{
	PrimaryActorTick.bCanEverTick = false;

	SetCanBeDamaged(true);

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	WallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallMesh"));
	WallMesh->SetupAttachment(Root);
	WallMesh->SetCollisionProfileName(TEXT("BlockAll"));

	HeartHealth = CreateDefaultSubobject<UHeartHealthComponent>(TEXT("HeartHealth"));
	HeartHealth->MaxHearts = 1;
	HeartHealth->StartingHearts = 1;
	HeartHealth->bDestroyOwnerOnDeath = true;
}

void ACrackedWall::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(HeartHealth))
	{
		HeartHealth->OnDeath.AddDynamic(this, &ACrackedWall::HandleWallDestroyed);
	}
}

void ACrackedWall::HandleWallDestroyed()
{
	// HeartHealth destroys owner by default; ensure collision is off immediately if BP overrides destroy.
	SetActorEnableCollision(false);
	SetActorHiddenInGame(true);
}
