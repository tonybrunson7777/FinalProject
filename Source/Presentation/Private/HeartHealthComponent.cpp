#include "HeartHealthComponent.h"

#include "GameFramework/Actor.h"
#include "TimerManager.h"

UHeartHealthComponent::UHeartHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHeartHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	const int32 InitialHearts = (StartingHearts > 0) ? StartingHearts : MaxHearts;
	CurrentHearts = FMath::Clamp(InitialHearts, 0, MaxHearts);
	BroadcastHeartsChanged();

	if (AActor* OwnerActor = GetOwner())
	{
		OwnerActor->OnTakeAnyDamage.AddDynamic(this, &UHeartHealthComponent::HandleTakeAnyDamage);
	}
}

void UHeartHealthComponent::ApplyHeartDamage(int32 HeartsToLose)
{
	if (HeartsToLose <= 0 || IsDead())
	{
		return;
	}

	CurrentHearts = FMath::Max(CurrentHearts - HeartsToLose, 0);
	BroadcastHeartsChanged();

	if (IsDead())
	{
		OnDeath.Broadcast();

		if (bDestroyOwnerOnDeath)
		{
			if (AActor* OwnerActor = GetOwner())
			{
				OwnerActor->Destroy();
			}
		}
	}
}

void UHeartHealthComponent::HealHearts(int32 HeartsToHeal)
{
	if (HeartsToHeal <= 0 || IsDead())
	{
		return;
	}

	CurrentHearts = FMath::Clamp(CurrentHearts + HeartsToHeal, 0, MaxHearts);
	BroadcastHeartsChanged();
}

bool UHeartHealthComponent::IsDead() const
{
	return CurrentHearts <= 0;
}

void UHeartHealthComponent::SetDamageImmune(bool bImmune)
{
	bIsDamageImmune = bImmune;

	if (!bImmune)
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(DamageImmunityTimerHandle);
		}
	}
}

void UHeartHealthComponent::SetDamageImmuneForDuration(float Duration)
{
	if (Duration <= 0.0f)
	{
		SetDamageImmune(false);
		return;
	}

	SetDamageImmune(true);

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(DamageImmunityTimerHandle);
		World->GetTimerManager().SetTimer(
			DamageImmunityTimerHandle,
			this,
			&UHeartHealthComponent::ClearDamageImmunity,
			Duration,
			false
		);
	}
}

void UHeartHealthComponent::ClearDamageImmunity()
{
	bIsDamageImmune = false;
}

void UHeartHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (bIsDamageImmune || Damage <= 0.0f)
	{
		return;
	}

	// Treat incoming damage as "hearts to lose" so 1.0f damage = 1 heart.
	const int32 HeartsToLose = FMath::Max(1, FMath::CeilToInt(Damage));
	ApplyHeartDamage(HeartsToLose);
}

void UHeartHealthComponent::BroadcastHeartsChanged()
{
	OnHeartsChanged.Broadcast(CurrentHearts, MaxHearts);
}
