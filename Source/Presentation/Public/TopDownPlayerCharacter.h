#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Blueprint/UserWidget.h"
#include "BombInventoryComponent.h"
#include "TopDownPlayerCharacter.generated.h"


UCLASS(Blueprintable)
class PRESENTATION_API ATopDownPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

	// HUD reference
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UUserWidget> m_cPlayerHUD;


public:
	ATopDownPlayerCharacter();

	

	UFUNCTION(BlueprintPure, Category = "Health")
	int32 GetCurrentHearts() const;

	//Reference to BombInventoryComponent Bomb Count
	UFUNCTION(BlueprintPure, Category = "Bomb")
	int32 GetCurrentBombCount() const;

	UFUNCTION(BlueprintCallable, Category = "Death")
	void GoToMainMenuFromDeath();

	UFUNCTION(BlueprintCallable, Category = "Death")
	void RestartLevelFromDeath();

protected:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TopDown|Debug")
	bool bEnableMovementDebug = false;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TopDown|Death")
	TSubclassOf<class UPlayerDeathMenuWidget> DeathMenuWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TopDown|Death")
	FName MainMenuLevelName = FName(TEXT("presentationMap"));

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TopDown|Death")
	FName RestartLevelName = NAME_None;

	UPROPERTY(BlueprintReadOnly, Category = "TopDown|Death")
	bool bIsPlayerDead = false;

	UPROPERTY(BlueprintReadOnly, Category = "TopDown|Death")
	TObjectPtr<class UPlayerDeathMenuWidget> DeathMenuWidget;

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

	UFUNCTION()
	void HandlePlayerDeath();

	void StartRoll();
	void UpdateRoll(float DeltaSeconds);
	void EndRoll();
	void EndRollCooldown();

	void RestoreGameplayInput();

	FVector RollStartLocation = FVector::ZeroVector;
	FVector RollEndLocation = FVector::ZeroVector;
	float RollElapsedTime = 0.0f;

	FTimerHandle RollCooldownTimerHandle;
};
