// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Spikes.generated.h"

class UStaticMeshComponent;

UCLASS()
class PRESENTATION_API ASpikes : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpikes();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* SpikeMesh;

	UPROPERTY(EditAnywhere, Category = "Spike Settings")
	float MoveDistance;

	UPROPERTY(EditAnywhere, Category = "Spike Settings")
	float InterpolationSpeed;

	UPROPERTY(EditAnywhere, Category = "Spike Settings")
	float CycleDelay;

	UPROPERTY(EditDefaultsOnly, Category = "Spike Settings")
	bool bIsActive;

	bool bHasChangedStates;

	float TargetLocation = 0.f;

	FTimerHandle CycleTimerHandle;

	UFUNCTION()
	void ToggleSpikeState();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
