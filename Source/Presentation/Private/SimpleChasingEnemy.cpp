#include "SimpleChasingEnemy.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "HeartHealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

ASimpleChasingEnemy::ASimpleChasingEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	HeartHealth = CreateDefaultSubobject<UHeartHealthComponent>(TEXT("HeartHealth"));
	HeartHealth->MaxHearts = 1;
	HeartHealth->StartingHearts = 1;
	HeartHealth->bDestroyOwnerOnDeath = true;

	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->MaxWalkSpeed = ChaseSpeed;
	}
}

void ASimpleChasingEnemy::BeginPlay()
{
	Super::BeginPlay();
	AcquireTarget();
}

void ASimpleChasingEnemy::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	AcquireTarget();
	UpdateChaseAndAttackState(DeltaSeconds);
}

void ASimpleChasingEnemy::AcquireTarget()
{
	if (IsValid(TargetActor))
	{
		return;
	}

	if (bTargetPlayerZero)
	{
		if (APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0))
		{
			TargetActor = PlayerPawn;
		}
	}
}

void ASimpleChasingEnemy::UpdateChaseAndAttackState(float DeltaSeconds)
{
	if (!IsValid(TargetActor))
	{
		StopAttacking();
		return;
	}

	const FVector ToTarget = TargetActor->GetActorLocation() - GetActorLocation();
	if (!IsTargetInViewAngle(ToTarget))
	{
		StopAttacking();
		return;
	}
	
	FaceTarget(ToTarget, DeltaSeconds);

	const float DistanceToTarget = ToTarget.Size();
	const bool bInAttackRange = DistanceToTarget <= AttackRange;

	if (!bInAttackRange)
	{
		const FVector MoveDirection = ToTarget.GetSafeNormal2D();
		AddMovementInput(MoveDirection, 1.0f);

		StopAttacking();
	}
	else
	{
		if (UCharacterMovementComponent* Movement = GetCharacterMovement())
		{
			Movement->StopMovementImmediately();
		}
		StartAttacking();
	}
}

bool ASimpleChasingEnemy::IsTargetInViewAngle(const FVector& ToTarget) const
{
	const FVector FlatToTarget = FVector(ToTarget.X, ToTarget.Y, 0.0f).GetSafeNormal();
	if (FlatToTarget.IsNearlyZero())
	{
		return true;
	}

	const FVector Forward = GetActorForwardVector().GetSafeNormal2D();
	const float Dot = FVector::DotProduct(Forward, FlatToTarget);
	const float HalfViewRadians = FMath::DegreesToRadians(FMath::Clamp(ViewAngleDegrees, 1.0f, 180.0f) * 0.5f);
	const float CosThreshold = FMath::Cos(HalfViewRadians);
	return Dot >= CosThreshold;
}

void ASimpleChasingEnemy::FaceTarget(const FVector& ToTarget, float DeltaSeconds)
{
	const FVector FlatToTarget = FVector(ToTarget.X, ToTarget.Y, 0.0f);
	if (FlatToTarget.IsNearlyZero())
	{
		return;
	}

	const FRotator CurrentRotation = GetActorRotation();
	const FRotator DesiredRotation = FlatToTarget.Rotation();
	const FRotator NewRotation = FMath::RInterpConstantTo(CurrentRotation, DesiredRotation, DeltaSeconds, TurnSpeedDegreesPerSecond);
	SetActorRotation(FRotator(0.0f, NewRotation.Yaw, 0.0f));
}

void ASimpleChasingEnemy::StartAttacking()
{
	if (GetWorldTimerManager().IsTimerActive(AttackTimerHandle))
	{
		return;
	}

	AttackTarget();
	GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &ASimpleChasingEnemy::AttackTarget, FMath::Max(AttackInterval, KINDA_SMALL_NUMBER), true);
}

void ASimpleChasingEnemy::StopAttacking()
{
	GetWorldTimerManager().ClearTimer(AttackTimerHandle);
}

void ASimpleChasingEnemy::AttackTarget()
{
	if (!IsValid(TargetActor))
	{
		StopAttacking();
		return;
	}

	const float DistanceToTarget = FVector::Dist(GetActorLocation(), TargetActor->GetActorLocation());
	const FVector ToTarget = TargetActor->GetActorLocation() - GetActorLocation();
	if (DistanceToTarget > AttackRange || !IsTargetInViewAngle(ToTarget))
	{
		StopAttacking();
		return;
	}

	UGameplayStatics::ApplyDamage(TargetActor, AttackDamage, GetController(), this, nullptr);
}
