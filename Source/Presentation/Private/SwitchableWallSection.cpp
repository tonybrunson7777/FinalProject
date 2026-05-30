#include "SwitchableWallSection.h"

#include "Components/StaticMeshComponent.h"

ASwitchableWallSection::ASwitchableWallSection()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	WallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallMesh"));
	WallMesh->SetupAttachment(Root);
	WallMesh->SetCollisionProfileName(TEXT("BlockAll"));
}

void ASwitchableWallSection::BeginPlay()
{
	Super::BeginPlay();
	SetWallActive(bStartsActive);
}

void ASwitchableWallSection::SetWallActive(bool bActive)
{
	bIsActive = bActive;

	SetActorHiddenInGame(!bActive);
	SetActorEnableCollision(bActive);

	if (IsValid(WallMesh))
	{
		WallMesh->SetVisibility(bActive, true);
		WallMesh->SetCollisionEnabled(bActive ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
	}
}
