#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SimpleChasingEnemy.generated.h"

UCLASS(Blueprintable)
class PRESENTATION_API ASimpleChasingEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	ASimpleChasingEnemy();

	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|Health")
	TObjectPtr<class UHeartHealthComponent> HeartHealth;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Movement")
	float ChaseSpeed = 260.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Movement")
	float TurnSpeedDegreesPerSecond = 540.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Combat")
	float AttackRange = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Combat")
	float AttackDamage = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Combat")
	float AttackInterval = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Targeting")
	bool bTargetPlayerZero = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Targeting", meta = (ClampMin = "1.0", ClampMax = "180.0", UIMin = "1.0", UIMax = "180.0"))
	float ViewAngleDegrees = 45.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Enemy|Targeting")
	TObjectPtr<AActor> TargetActor;

	FTimerHandle AttackTimerHandle;

	void AcquireTarget();
	void UpdateChaseAndAttackState(float DeltaSeconds);
	bool IsTargetInViewAngle(const FVector& ToTarget) const;
	void FaceTarget(const FVector& ToTarget, float DeltaSeconds);
	void StartAttacking();
	void StopAttacking();
	void AttackTarget();
};
