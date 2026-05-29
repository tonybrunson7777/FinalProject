#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TopDownPlayerCharacter.generated.h"

UCLASS(Blueprintable)
class PRESENTATION_API ATopDownPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ATopDownPlayerCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TopDown|Debug")
	bool bEnableMovementDebug = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TopDown|Camera")
	TObjectPtr<class USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TopDown|Camera")
	TObjectPtr<class UCameraComponent> TopDownCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TopDown|Interaction")
	TObjectPtr<class UPlayerInteractionComponent> PlayerInteraction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TopDown|Bomb")
	TObjectPtr<class UBombInventoryComponent> BombInventory;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TopDown|Health")
	TObjectPtr<class UHeartHealthComponent> HeartHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TopDown|Keys")
	TObjectPtr<class UVirtualKeyInventoryComponent> VirtualKeyInventory;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TopDown|Movement")
	bool bUseEightDirectionFacing = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TopDown|Roll", meta = (ClampMin = "1.0", UIMin = "1.0"))
	float RollDistance = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TopDown|Roll", meta = (ClampMin = "0.05", UIMin = "0.05"))
	float RollDuration = 0.35f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TopDown|Roll", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float RollInvulnerabilityDuration = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TopDown|Roll", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float RollCooldownDuration = 2.0f;

	UPROPERTY(BlueprintReadOnly, Category = "TopDown|Roll")
	bool bIsRolling = false;

	UPROPERTY(BlueprintReadOnly, Category = "TopDown|Roll")
	bool bRollOnCooldown = false;

	float LastMoveForwardValue = 0.0f;
	float LastMoveRightValue = 0.0f;
	float DebugElapsedTime = 0.0f;
	FVector2D CurrentMoveInput = FVector2D::ZeroVector;

	UFUNCTION()
	void MoveForward(float Value);

	UFUNCTION()
	void MoveRight(float Value);

	UFUNCTION()
	void HandleInteractPressed();

	UFUNCTION()
	void HandlePlaceBombPressed();

	UFUNCTION()
	void HandleRollPressed();

	void StartRoll();
	void UpdateRoll(float DeltaSeconds);
	void EndRoll();
	void EndRollCooldown();

	FVector RollStartLocation = FVector::ZeroVector;
	FVector RollEndLocation = FVector::ZeroVector;
	float RollElapsedTime = 0.0f;

	FTimerHandle RollCooldownTimerHandle;
};
