#include "StationaryAreaEnemy.h"

#include "Components/SkeletalMeshComponent.h"
#include "HeartHealthComponent.h"
#include "Engine/EngineTypes.h"
#include "Kismet/KismetSystemLibrary.h"
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
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	TArray<AActor*> OverlappedActors;
	if (!UKismetSystemLibrary::SphereOverlapActors(
		World,
		GetActorLocation(),
		DamageRadius,
		ObjectTypes,
		AActor::StaticClass(),
		ActorsToIgnore,
		OverlappedActors))
	{
		return;
	}

	const int32 HeartsToLose = FMath::Max(1, FMath::CeilToInt(DamageAmount));
	for (AActor* OverlappedActor : OverlappedActors)
	{
		if (UHeartHealthComponent* TargetHealth = OverlappedActor->FindComponentByClass<UHeartHealthComponent>())
		{
			if (!TargetHealth->IsDead() && !TargetHealth->IsDamageImmune())
			{
				TargetHealth->ApplyHeartDamage(HeartsToLose, this, TEXT("PlantAura"));
			}
		}
	}
}
