#include "DamageTrap.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

ADamageTrap::ADamageTrap()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	TrapMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TrapMesh"));
	TrapMesh->SetupAttachment(Root);
	TrapMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(Root);
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionObjectType(ECC_WorldDynamic);
	TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	TriggerBox->SetGenerateOverlapEvents(true);
	TriggerBox->SetBoxExtent(FVector(100.0f, 100.0f, 40.0f));
}

void ADamageTrap::BeginPlay()
{
	Super::BeginPlay();

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ADamageTrap::HandleOverlapBegin);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &ADamageTrap::HandleOverlapEnd);
}

void ADamageTrap::HandleOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValid(OtherActor) || OtherActor == this || ActiveDamageTimers.Contains(OtherActor))
	{
		return;
	}

	const APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (OtherActor != PlayerPawn)
	{
		return;
	}

	if (bDamageOnBeginOverlap)
	{
		ApplyDamageToActor(OtherActor);
	}

	FTimerHandle DamageTimerHandle;
	FTimerDelegate DamageDelegate;
	DamageDelegate.BindLambda([this, WeakTarget = TWeakObjectPtr<AActor>(OtherActor)]()
	{
		if (WeakTarget.IsValid())
		{
			ApplyDamageToActor(WeakTarget.Get());
		}
	});

	GetWorldTimerManager().SetTimer(DamageTimerHandle, DamageDelegate, FMath::Max(DamageInterval, KINDA_SMALL_NUMBER), true);
	ActiveDamageTimers.Add(OtherActor, DamageTimerHandle);
}

void ADamageTrap::HandleOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!IsValid(OtherActor))
	{
		return;
	}

	if (FTimerHandle* ExistingTimer = ActiveDamageTimers.Find(OtherActor))
	{
		GetWorldTimerManager().ClearTimer(*ExistingTimer);
		ActiveDamageTimers.Remove(OtherActor);
	}
}

void ADamageTrap::ApplyDamageToActor(AActor* TargetActor)
{
	if (!IsValid(TargetActor))
	{
		return;
	}

	UGameplayStatics::ApplyDamage(TargetActor, DamageAmount, nullptr, this, nullptr);
}
