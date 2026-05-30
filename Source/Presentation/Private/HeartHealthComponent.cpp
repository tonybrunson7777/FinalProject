#include "HeartHealthComponent.h"

#include "Engine/Engine.h"
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

void UHeartHealthComponent::LogDamageDebug(const FString& Event, int32 HeartsToLose, AActor* DamageCauser, FName DamageSource) const
{
	if (!bEnableDamageDebug)
	{
		return;
	}

	// const FString SourceLabel = DamageSource.IsNone() ? TEXT("Unknown") : DamageSource.ToString();
	// const FString CauserLabel = GetNameSafe(DamageCauser);
	// const FString OwnerLabel = GetNameSafe(GetOwner());
	// const FString Message = FString::Printf(
	// 	TEXT("[%s] %s lost %d heart(s) from '%s' via %s | Now %d/%d"),
	// 	*Event,
	// 	*OwnerLabel,
	// 	HeartsToLose,
	// 	*CauserLabel,
	// 	*SourceLabel,
	// 	CurrentHearts,
	// 	MaxHearts
	// );
	// UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);
	// if (GEngine)
	// {
	// 	GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Red, Message);
	// }
}

void UHeartHealthComponent::ApplyHeartDamage(int32 HeartsToLose, AActor* DamageCauser, FName DamageSource)
{
	if (HeartsToLose <= 0)
	{
		return;
	}

	if (IsDead())
	{
		// if (bEnableDamageDebug)
		// {
		// 	UE_LOG(LogTemp, Warning, TEXT("[DamageBlocked] %s is dead - ignored %d heart(s) from '%s'."),
		// 		*GetNameSafe(GetOwner()), HeartsToLose, *GetNameSafe(DamageCauser));
		// }
		return;
	}

	if (bIsDamageImmune)
	{
		// if (bEnableDamageDebug)
		// {
		// 	const FString Message = FString::Printf(
		// 		TEXT("[DamageBlocked] %s is immune - blocked %d heart(s) from '%s' via %s"),
		// 		*GetNameSafe(GetOwner()),
		// 		HeartsToLose,
		// 		*GetNameSafe(DamageCauser),
		// 		DamageSource.IsNone() ? TEXT("Unknown") : *DamageSource.ToString()
		// 	);
		// 	UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);
		// 	if (GEngine)
		// 	{
		// 		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, Message);
		// 	}
		// }
		return;
	}

	const int32 HeartsBefore = CurrentHearts;
	CurrentHearts = FMath::Max(CurrentHearts - HeartsToLose, 0);
	BroadcastHeartsChanged();

	LogDamageDebug(TEXT("DamageTaken"), HeartsBefore - CurrentHearts, DamageCauser, DamageSource);

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

	// if (bEnableDamageDebug)
	// {
	// 	const FString Message = FString::Printf(
	// 		TEXT("[Healed] %s restored %d heart(s) | Now %d/%d"),
	// 		*GetNameSafe(GetOwner()),
	// 		HeartsToHeal,
	// 		CurrentHearts,
	// 		MaxHearts
	// 	);
	// 	UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);
	// 	if (GEngine)
	// 	{
	// 		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, Message);
	// 	}
	// }
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

	// if (bEnableDamageDebug)
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("[DamageImmune] %s immune for %.2f seconds."), *GetNameSafe(GetOwner()), Duration);
	// }

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

	// if (bEnableDamageDebug)
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("[DamageImmune] %s immunity ended."), *GetNameSafe(GetOwner()));
	// }
}

void UHeartHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (bIsDamageImmune || Damage <= 0.0f)
	{
		return;
	}

	const int32 HeartsToLose = FMath::Max(1, FMath::CeilToInt(Damage));
	ApplyHeartDamage(HeartsToLose, DamageCauser, TEXT("ApplyDamage"));
}

void UHeartHealthComponent::BroadcastHeartsChanged()
{
	OnHeartsChanged.Broadcast(CurrentHearts, MaxHearts);
}
