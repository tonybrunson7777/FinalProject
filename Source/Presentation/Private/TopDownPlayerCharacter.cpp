#include "TopDownPlayerCharacter.h"

#include "BombInventoryComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "HeartHealthComponent.h"
#include "PlayerInteractionComponent.h"
#include "VirtualKeyInventoryComponent.h"
#include "TimerManager.h"

ATopDownPlayerCharacter::ATopDownPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		// Zelda-style top-down movement
		Movement->bOrientRotationToMovement = false;
		Movement->bUseControllerDesiredRotation = false;
		Movement->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
		Movement->MaxWalkSpeed = 450.0f;
	}

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 900.0f;
	CameraBoom->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->bUsePawnControlRotation = false;
	CameraBoom->bInheritPitch = false;
	CameraBoom->bInheritYaw = false;
	CameraBoom->bInheritRoll = false;

	TopDownCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCamera->bUsePawnControlRotation = false;

	PlayerInteraction = CreateDefaultSubobject<UPlayerInteractionComponent>(TEXT("PlayerInteraction"));
	BombInventory = CreateDefaultSubobject<UBombInventoryComponent>(TEXT("BombInventory"));
	HeartHealth = CreateDefaultSubobject<UHeartHealthComponent>(TEXT("HeartHealth"));
	VirtualKeyInventory = CreateDefaultSubobject<UVirtualKeyInventoryComponent>(TEXT("VirtualKeyInventory"));

	HeartHealth->MaxHearts = 4;
	HeartHealth->StartingHearts = 4;
	HeartHealth->bDestroyOwnerOnDeath = false;
}

void ATopDownPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (!bEnableMovementDebug || !GEngine)
	{
		return;
	}

	const bool bHasController = (GetController() != nullptr);
	GEngine->AddOnScreenDebugMessage(
		1001,
		5.0f,
		bHasController ? FColor::Green : FColor::Red,
		FString::Printf(TEXT("TopDown BeginPlay - Possessed: %s"), bHasController ? TEXT("YES") : TEXT("NO"))
	);
}

void ATopDownPlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bIsRolling)
	{
		UpdateRoll(DeltaSeconds);
		return;
	}

	CurrentMoveInput = FVector2D(LastMoveRightValue, LastMoveForwardValue);
	if (bUseEightDirectionFacing && CurrentMoveInput.SizeSquared() > KINDA_SMALL_NUMBER)
	{
		const float InputYaw = FMath::RadiansToDegrees(FMath::Atan2(CurrentMoveInput.X, CurrentMoveInput.Y));
		const float SnappedYaw = FMath::RoundToFloat(InputYaw / 45.0f) * 45.0f;
		SetActorRotation(FRotator(0.0f, SnappedYaw, 0.0f));
	}

	if (!bEnableMovementDebug || !GEngine)
	{
		return;
	}

	DebugElapsedTime += DeltaSeconds;
	if (DebugElapsedTime < 0.25f)
	{
		return;
	}

	DebugElapsedTime = 0.0f;

	const FVector Velocity = GetVelocity();
	const bool bHasController = (GetController() != nullptr);
	GEngine->AddOnScreenDebugMessage(
		1002,
		0.3f,
		FColor::Yellow,
		FString::Printf(
			TEXT("Input F: %.2f R: %.2f | Vel: %.1f | Possessed: %s"),
			LastMoveForwardValue,
			LastMoveRightValue,
			Velocity.Size2D(),
			bHasController ? TEXT("YES") : TEXT("NO")
		)
	);
}

void ATopDownPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ATopDownPlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ATopDownPlayerCharacter::MoveRight);
	PlayerInputComponent->BindAction(TEXT("Interact"), IE_Pressed, this, &ATopDownPlayerCharacter::HandleInteractPressed);
	PlayerInputComponent->BindAction(TEXT("PlaceBomb"), IE_Pressed, this, &ATopDownPlayerCharacter::HandlePlaceBombPressed);
	PlayerInputComponent->BindAction(TEXT("Roll"), IE_Pressed, this, &ATopDownPlayerCharacter::HandleRollPressed);

	if (bEnableMovementDebug && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(1003, 4.0f, FColor::Green, TEXT("TopDown Input Bindings: OK"));
	}
}

void ATopDownPlayerCharacter::MoveForward(float Value)
{
	LastMoveForwardValue = Value;

	if (bIsRolling)
	{
		return;
	}

	if (FMath::IsNearlyZero(Value))
	{
		return;
	}

	AddMovementInput(FVector::ForwardVector, Value);
}

void ATopDownPlayerCharacter::MoveRight(float Value)
{
	LastMoveRightValue = Value;

	if (bIsRolling)
	{
		return;
	}

	if (FMath::IsNearlyZero(Value))
	{
		return;
	}

	AddMovementInput(FVector::RightVector, Value);
}

void ATopDownPlayerCharacter::HandleInteractPressed()
{
	if (IsValid(PlayerInteraction))
	{
		PlayerInteraction->Interact();
	}
}

void ATopDownPlayerCharacter::HandlePlaceBombPressed()
{
	if (IsValid(PlayerInteraction))
	{
		PlayerInteraction->PlaceBomb();
	}
}

void ATopDownPlayerCharacter::HandleRollPressed()
{
	if (bIsRolling || bRollOnCooldown)
	{
		return;
	}

	StartRoll();
}

void ATopDownPlayerCharacter::StartRoll()
{
	const FVector RollDirection = GetActorForwardVector().GetSafeNormal2D();
	if (RollDirection.IsNearlyZero())
	{
		return;
	}

	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->StopMovementImmediately();
	}

	RollStartLocation = GetActorLocation();
	RollEndLocation = RollStartLocation + RollDirection * RollDistance;
	RollElapsedTime = 0.0f;
	bIsRolling = true;

	if (IsValid(HeartHealth))
	{
		HeartHealth->SetDamageImmuneForDuration(RollInvulnerabilityDuration);
	}

	bRollOnCooldown = true;
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(RollCooldownTimerHandle);
		World->GetTimerManager().SetTimer(
			RollCooldownTimerHandle,
			this,
			&ATopDownPlayerCharacter::EndRollCooldown,
			FMath::Max(RollCooldownDuration, KINDA_SMALL_NUMBER),
			false
		);
	}
}

void ATopDownPlayerCharacter::UpdateRoll(float DeltaSeconds)
{
	RollElapsedTime += DeltaSeconds;
	const float Alpha = FMath::Clamp(RollElapsedTime / FMath::Max(RollDuration, KINDA_SMALL_NUMBER), 0.0f, 1.0f);
	const FVector NewLocation = FMath::Lerp(RollStartLocation, RollEndLocation, Alpha);

	FHitResult Hit;
	SetActorLocation(NewLocation, true, &Hit);

	if (Alpha >= 1.0f || Hit.bBlockingHit)
	{
		EndRoll();
	}
}

void ATopDownPlayerCharacter::EndRoll()
{
	bIsRolling = false;
	RollElapsedTime = 0.0f;

	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->StopMovementImmediately();
	}
}

void ATopDownPlayerCharacter::EndRollCooldown()
{
	bRollOnCooldown = false;
}
