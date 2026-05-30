#include "BossEncounterEnemy.h"

#include "EnemyLineOfSight.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HeartHealthComponent.h"
#include "HeartHealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

ABossEncounterEnemy::ABossEncounterEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	HeartHealth = CreateDefaultSubobject<UHeartHealthComponent>(TEXT("HeartHealth"));
	HeartHealth->MaxHearts = 6;
	HeartHealth->StartingHearts = 6;
	HeartHealth->bDestroyOwnerOnDeath = true;

	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->bOrientRotationToMovement = true;
		Movement->MaxWalkSpeed = NormalSpeed;
	}
}

void ABossEncounterEnemy::BeginPlay()
{
	Super::BeginPlay();

	AcquireTarget();
	EnterJumpPhase();

	GetWorldTimerManager().SetTimer(
		PhaseSwitchTimerHandle,
		this,
		&ABossEncounterEnemy::TogglePhase,
		FMath::Max(PhaseSwitchInterval, KINDA_SMALL_NUMBER),
		true
	);
}

void ABossEncounterEnemy::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	AcquireTarget();

	if (CurrentPhase == EBossPhase::Charging)
	{
		UpdateChargePhase(DeltaSeconds);
	}
}

void ABossEncounterEnemy::AcquireTarget()
{
	if (IsValid(TargetActor))
	{
		return;
	}

	if (bTargetPlayerZero)
	{
		TargetActor = UGameplayStatics::GetPlayerPawn(this, 0);
	}
}

void ABossEncounterEnemy::TogglePhase()
{
	if (CurrentPhase == EBossPhase::Jumping)
	{
		EnterChargePhase();
	}
	else
	{
		EnterJumpPhase();
	}
}

void ABossEncounterEnemy::EnterJumpPhase()
{
	CurrentPhase = EBossPhase::Jumping;

	GetWorldTimerManager().ClearTimer(ChargeDamageTimerHandle);

	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->MaxWalkSpeed = NormalSpeed;
		Movement->StopMovementImmediately();
	}

	GetWorldTimerManager().ClearTimer(JumpTimerHandle);
	PerformJump();
	GetWorldTimerManager().SetTimer(
		JumpTimerHandle,
		this,
		&ABossEncounterEnemy::PerformJump,
		FMath::Max(JumpInterval, KINDA_SMALL_NUMBER),
		true
	);
}

void ABossEncounterEnemy::EnterChargePhase()
{
	CurrentPhase = EBossPhase::Charging;

	GetWorldTimerManager().ClearTimer(JumpTimerHandle);

	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->MaxWalkSpeed = ChargeSpeed;
	}
}

void ABossEncounterEnemy::PerformJump()
{
	const FVector RandomOffset = FVector(
		FMath::FRandRange(-JumpRadius, JumpRadius),
		FMath::FRandRange(-JumpRadius, JumpRadius),
		0.0f
	);

	FVector JumpDirection = RandomOffset.GetSafeNormal2D();
	if (IsValid(TargetActor) && PresentationEnemyVisibility::HasLineOfSightTo(this, TargetActor))
	{
		const FVector TowardPlayer = (TargetActor->GetActorLocation() - GetActorLocation()).GetSafeNormal2D();
		JumpDirection = (JumpDirection + TowardPlayer * 0.5f).GetSafeNormal2D();
	}

	const FVector LaunchVelocity = JumpDirection * JumpHorizontalSpeed + FVector(0.0f, 0.0f, JumpVerticalSpeed);
	LaunchCharacter(LaunchVelocity, true, true);
}

void ABossEncounterEnemy::UpdateChargePhase(float DeltaSeconds)
{
	if (!IsValid(TargetActor) || !PresentationEnemyVisibility::HasLineOfSightTo(this, TargetActor))
	{
		if (UCharacterMovementComponent* Movement = GetCharacterMovement())
		{
			Movement->StopMovementImmediately();
		}
		return;
	}

	const FVector ToTarget = TargetActor->GetActorLocation() - GetActorLocation();
	const FVector MoveDirection = ToTarget.GetSafeNormal2D();
	if (!MoveDirection.IsNearlyZero())
	{
		AddMovementInput(MoveDirection, 1.0f);
		FaceDirection(MoveDirection, DeltaSeconds);
	}

	const float DistanceToTarget = ToTarget.Size2D();
	if (DistanceToTarget <= ChargeHitRange)
	{
		TryApplyChargeDamage();
	}
}

void ABossEncounterEnemy::FaceDirection(const FVector& Direction, float DeltaSeconds)
{
	const FRotator DesiredRotation = Direction.Rotation();
	const FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), DesiredRotation, DeltaSeconds, 12.0f);
	SetActorRotation(FRotator(0.0f, NewRotation.Yaw, 0.0f));
}

void ABossEncounterEnemy::TryApplyChargeDamage()
{
	if (!IsValid(TargetActor)
		|| !PresentationEnemyVisibility::HasLineOfSightTo(this, TargetActor)
		|| GetWorldTimerManager().IsTimerActive(ChargeDamageTimerHandle))
	{
		return;
	}

	if (UHeartHealthComponent* TargetHealth = TargetActor->FindComponentByClass<UHeartHealthComponent>())
	{
		const int32 HeartsToLose = FMath::Max(1, FMath::CeilToInt(ChargeDamage));
		TargetHealth->ApplyHeartDamage(HeartsToLose);
	}

	GetWorldTimerManager().SetTimer(
		ChargeDamageTimerHandle,
		this,
		&ABossEncounterEnemy::ResetChargeDamageCooldown,
		FMath::Max(ChargeDamageCooldown, KINDA_SMALL_NUMBER),
		false
	);
}

void ABossEncounterEnemy::ResetChargeDamageCooldown()
{
}
