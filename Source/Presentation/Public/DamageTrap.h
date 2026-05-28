#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DamageTrap.generated.h"

class UBoxComponent;
class UPrimitiveComponent;
class UStaticMeshComponent;
struct FHitResult;

UCLASS(Blueprintable)
class PRESENTATION_API ADamageTrap : public AActor
{
	GENERATED_BODY()

public:
	ADamageTrap();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trap")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trap")
	TObjectPtr<UBoxComponent> TriggerBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trap")
	TObjectPtr<UStaticMeshComponent> TrapMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trap|Damage")
	float DamageAmount = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trap|Damage")
	float DamageInterval = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trap|Damage")
	bool bDamageOnBeginOverlap = true;

	UPROPERTY()
	TMap<TObjectPtr<AActor>, FTimerHandle> ActiveDamageTimers;

	UFUNCTION()
	void HandleOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void HandleOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void ApplyDamageToActor(AActor* TargetActor);
};
