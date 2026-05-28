#include "BombInventoryComponent.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "Engine/World.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundBase.h"
#include "TimerManager.h"

UBombInventoryComponent::UBombInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBombInventoryComponent::AddBomb(TSubclassOf<AActor> BombClass, int32 CountToAdd)
{
	if (CountToAdd <= 0 || !*BombClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("AddBomb failed: invalid count/class. Count=%d Class='%s'"), CountToAdd, *GetNameSafe(BombClass.Get()));
		return;
	}

	HeldBombClass = BombClass;
	BombCount += CountToAdd;
	UE_LOG(LogTemp, Warning, TEXT("AddBomb success: +%d '%s'. New BombCount=%d"), CountToAdd, *GetNameSafe(BombClass.Get()), BombCount);
}

bool UBombInventoryComponent::TryPlaceBomb()
{
	AActor* OwnerActor = GetOwner();
	if (!IsValid(OwnerActor) || BombCount <= 0 || !*HeldBombClass)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("TryPlaceBomb failed: owner='%s' BombCount=%d HeldBombClass='%s'"),
			*GetNameSafe(OwnerActor),
			BombCount,
			*GetNameSafe(HeldBombClass.Get())
		);
		return false;
	}

	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		UE_LOG(LogTemp, Warning, TEXT("TryPlaceBomb failed: world invalid."));
		return false;
	}

	const FVector SpawnLocation = OwnerActor->GetActorLocation() + OwnerActor->GetActorForwardVector() * PlaceDistance;
	const FRotator SpawnRotation = OwnerActor->GetActorRotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	SpawnParams.Owner = OwnerActor;
	SpawnParams.Instigator = Cast<APawn>(OwnerActor);

	AActor* SpawnedBomb = World->SpawnActor<AActor>(HeldBombClass, SpawnLocation, SpawnRotation, SpawnParams);
	if (!IsValid(SpawnedBomb))
	{
		UE_LOG(LogTemp, Warning, TEXT("TryPlaceBomb failed: SpawnActor failed for class '%s'."), *GetNameSafe(HeldBombClass.Get()));
		return false;
	}

	ScheduleBombDetonation(SpawnedBomb);

	BombCount = FMath::Max(0, BombCount - 1);
	UE_LOG(LogTemp, Warning, TEXT("TryPlaceBomb success: spawned '%s'. Remaining BombCount=%d"), *GetNameSafe(SpawnedBomb), BombCount);
	if (BombCount == 0)
	{
		HeldBombClass = nullptr;
		UE_LOG(LogTemp, Warning, TEXT("TryPlaceBomb: inventory empty, HeldBombClass cleared."));
	}

	return true;
}

void UBombInventoryComponent::ScheduleBombDetonation(AActor* SpawnedBomb)
{
	if (!IsValid(SpawnedBomb))
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	FTimerHandle FuseTimerHandle;
	FTimerDelegate FuseDelegate;
	FuseDelegate.BindLambda([this, WeakBomb = TWeakObjectPtr<AActor>(SpawnedBomb)]()
	{
		if (!WeakBomb.IsValid())
		{
			return;
		}

		DetonateBomb(WeakBomb.Get());
	});

	World->GetTimerManager().SetTimer(FuseTimerHandle, FuseDelegate, FMath::Max(FuseSeconds, KINDA_SMALL_NUMBER), false);
	UE_LOG(LogTemp, Warning, TEXT("Bomb fuse started: '%s' will detonate in %.2f seconds."), *GetNameSafe(SpawnedBomb), FuseSeconds);
}

void UBombInventoryComponent::DetonateBomb(AActor* BombActor)
{
	if (!IsValid(BombActor))
	{
		return;
	}

	AActor* OwnerActor = GetOwner();
	AController* EventInstigator = nullptr;
	if (const APawn* OwnerPawn = Cast<APawn>(OwnerActor))
	{
		EventInstigator = OwnerPawn->GetController();
	}

	const FVector ExplosionLocation = BombActor->GetActorLocation();

	if (IsValid(ExplosionEffect))
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionEffect, ExplosionLocation, FRotator::ZeroRotator, FVector(1.0f), true);
	}

	if (IsValid(ExplosionSound))
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, ExplosionLocation);
	}

	UGameplayStatics::ApplyRadialDamage(
		this,
		ExplosionDamage,
		ExplosionLocation,
		ExplosionRadius,
		nullptr,
		TArray<AActor*>(),
		BombActor,
		EventInstigator,
		true
	);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Bomb detonated: location=%s radius=%.1f damage=%.1f"),
		*ExplosionLocation.ToCompactString(),
		ExplosionRadius,
		ExplosionDamage
	);

	BombActor->Destroy();
}
