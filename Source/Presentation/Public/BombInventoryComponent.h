#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BombInventoryComponent.generated.h"

class UParticleSystem;
class USoundBase;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PRESENTATION_API UBombInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBombInventoryComponent();

	UPROPERTY(BlueprintReadOnly, Category = "Bomb")
	int32 BombCount = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Bomb")
	TSubclassOf<AActor> HeldBombClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bomb|Placement")
	float PlaceDistance = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bomb|Explosion")
	float FuseSeconds = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bomb|Explosion")
	float ExplosionRadius = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bomb|Explosion")
	float ExplosionDamage = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bomb|Explosion")
	TObjectPtr<UParticleSystem> ExplosionEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bomb|Explosion")
	TObjectPtr<USoundBase> ExplosionSound;

	UFUNCTION(BlueprintCallable, Category = "Bomb")
	void AddBomb(TSubclassOf<AActor> BombClass, int32 CountToAdd = 1);

	UFUNCTION(BlueprintCallable, Category = "Bomb")
	bool TryPlaceBomb();

protected:
	void ScheduleBombDetonation(AActor* SpawnedBomb);
	void DetonateBomb(AActor* BombActor);
};
