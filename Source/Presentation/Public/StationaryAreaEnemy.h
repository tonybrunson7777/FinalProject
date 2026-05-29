#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StationaryAreaEnemy.generated.h"

UCLASS(Blueprintable)
class PRESENTATION_API AStationaryAreaEnemy : public AActor
{
	GENERATED_BODY()

public:
	AStationaryAreaEnemy();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy")
	TObjectPtr<class USkeletalMeshComponent> PlantMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|Health")
	TObjectPtr<class UHeartHealthComponent> HeartHealth;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Combat", meta = (ClampMin = "1.0", UIMin = "1.0"))
	float DamageRadius = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Combat", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float DamageAmount = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Combat", meta = (ClampMin = "0.1", UIMin = "0.1"))
	float DamageInterval = 2.0f;

	FTimerHandle DamageTimerHandle;

	void ApplyAreaDamage();
};
