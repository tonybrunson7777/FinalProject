#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BossEncounterEnemy.generated.h"

UENUM(BlueprintType)
enum class EBossPhase : uint8
{
	Jumping,
	Charging
};

UCLASS(Blueprintable)
class PRESENTATION_API ABossEncounterEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	ABossEncounterEnemy();

	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Health")
	TObjectPtr<class UHeartHealthComponent> HeartHealth;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Phase")
	float PhaseSwitchInterval = 3.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Boss|Phase")
	EBossPhase CurrentPhase = EBossPhase::Jumping;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Jump")
	float JumpHorizontalSpeed = 650.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Jump")
	float JumpVerticalSpeed = 420.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Jump")
	float JumpRadius = 350.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Jump")
	float JumpInterval = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Charge")
	float ChargeSpeed = 750.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Charge")
	float NormalSpeed = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Charge")
	float ChargeHitRange = 160.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Combat")
	float ChargeDamage = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Combat")
	float ChargeDamageCooldown = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Targeting")
	bool bTargetPlayerZero = true;

	UPROPERTY(BlueprintReadOnly, Category = "Boss|Targeting")
	TObjectPtr<AActor> TargetActor;

	FTimerHandle PhaseSwitchTimerHandle;
	FTimerHandle JumpTimerHandle;
	FTimerHandle ChargeDamageTimerHandle;

	void AcquireTarget();
	void TogglePhase();
	void EnterJumpPhase();
	void EnterChargePhase();
	void PerformJump();
	void UpdateChargePhase(float DeltaSeconds);
	void FaceDirection(const FVector& Direction, float DeltaSeconds);
	void TryApplyChargeDamage();
	void ResetChargeDamageCooldown();
};
