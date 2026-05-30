// Fill out your copyright notice in the Description page of Project Settings.


#include "Spikes.h"

// Sets default values
ASpikes::ASpikes() :
	MoveDistance(1.f),
	InterpolationSpeed(5.f),
	CycleDelay(2.f),
	bIsActive(true),
	bHasChangedStates(false)
{
	PrimaryActorTick.bCanEverTick = true;
	
	SpikeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SpikeMesh"));
	SpikeMesh->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
}

void ASpikes::BeginPlay()
{
	Super::BeginPlay();

	if (SpikeMesh && SpikeMesh->GetStaticMesh())
	{
		MoveDistance = SpikeMesh->Bounds.BoxExtent.Z * 2.f;
	}

	if (bIsActive)
	{
		TargetLocation = GetActorLocation().Z;
	}
	else
	{
		TargetLocation = GetActorLocation().Z - MoveDistance;
	}

	GetWorldTimerManager().SetTimer(CycleTimerHandle, this, &ASpikes::ToggleSpikeState, CycleDelay, true);
}

void ASpikes::ToggleSpikeState()
{
	bIsActive = !bIsActive;
	bHasChangedStates = true;
}

void ASpikes::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bHasChangedStates)
	{
		bHasChangedStates = false; 

		if (bIsActive)
		{
			TargetLocation += MoveDistance;
		}
		else
		{
			TargetLocation -= MoveDistance;
		}
	}

	FVector CurrentLocation = SpikeMesh->GetRelativeLocation();
	CurrentLocation.Z = FMath::FInterpTo(CurrentLocation.Z, TargetLocation, DeltaTime, InterpolationSpeed);

	SpikeMesh->SetRelativeLocation(CurrentLocation);
}