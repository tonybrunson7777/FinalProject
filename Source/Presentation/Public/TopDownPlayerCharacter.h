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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TopDown|Movement")
	bool bUseEightDirectionFacing = true;

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
};
