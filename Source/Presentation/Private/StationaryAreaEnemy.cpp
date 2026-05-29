#include "StationaryAreaEnemy.h"

#include "Components/SkeletalMeshComponent.h"
#include "HeartHealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

AStationaryAreaEnemy::AStationaryAreaEnemy()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	PlantMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PlantMesh"));
	PlantMesh->SetupAttachment(Root);
	PlantMesh->SetCollisionProfileName(TEXT("BlockAll"));

	HeartHealth = CreateDefaultSubobject<UHeartHealthComponent>(TEXT("HeartHealth"));
	HeartHealth->MaxHearts = 1;
	HeartHealth->StartingHearts = 1;
	HeartHealth->bDestroyOwnerOnDeath = true;
}

void AStationaryAreaEnemy::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(
		DamageTimerHandle,
		this,
		&AStationaryAreaEnemy::ApplyAreaDamage,
		FMath::Max(DamageInterval, KINDA_SMALL_NUMBER),
		true
	);
}

void AStationaryAreaEnemy::ApplyAreaDamage()
{
	UGameplayStatics::ApplyRadialDamage(
		this,
		DamageAmount,
		GetActorLocation(),
		DamageRadius,
		nullptr,
		TArray<AActor*>(),
		this,
		nullptr,
		true
	);
}
