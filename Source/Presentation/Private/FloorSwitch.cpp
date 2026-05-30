#include "FloorSwitch.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "SwitchableWallSection.h"

AFloorSwitch::AFloorSwitch()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(Root);
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionObjectType(ECC_WorldDynamic);
	TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	TriggerBox->SetGenerateOverlapEvents(true);
	TriggerBox->SetBoxExtent(FVector(80.0f, 80.0f, 20.0f));

	SwitchMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SwitchMesh"));
	SwitchMesh->SetupAttachment(Root);
	SwitchMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AFloorSwitch::BeginPlay()
{
	Super::BeginPlay();

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AFloorSwitch::HandleOverlapBegin);

	bIsActivated = bStartsActivated;
	ApplySwitchStateToWalls();
}

void AFloorSwitch::HandleOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValid(OtherActor) || OtherActor == this)
	{
		return;
	}

	const APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (OtherActor != PlayerPawn)
	{
		return;
	}

	ToggleSwitch();
}

void AFloorSwitch::ToggleSwitch()
{
	bIsActivated = !bIsActivated;
	ApplySwitchStateToWalls();
}

void AFloorSwitch::ApplySwitchStateToWalls()
{
	for (ASwitchableWallSection* WallSection : LinkedWallSections)
	{
		if (IsValid(WallSection))
		{
			WallSection->SetWallActive(bIsActivated);
		}
	}
}
