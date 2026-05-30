#include "TopDownPlayerCharacter.h"

#include "BombInventoryComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "HeartHealthComponent.h"
#include "PlayerInteractionComponent.h"
#include "Blueprint/UserWidget.h"

ATopDownPlayerCharacter::ATopDownPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		// Zelda-style top-down movement: keep facing stable, do not auto-rotate with input.
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

	if (m_cPlayerHUD != nullptr)
	{
		//Add HUD to Viewport
		UUserWidget* HUD = CreateWidget<UUserWidget>(Cast<APlayerController>(GetController()), m_cPlayerHUD);
		HUD->AddToViewport(9999);
	}
}

void ATopDownPlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (HeartHealth)
{
    m_fHealth = static_cast<float>(HeartHealth->CurrentHearts);
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

	if (bEnableMovementDebug && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(1003, 4.0f, FColor::Green, TEXT("TopDown Input Bindings: OK"));
	}
}

void ATopDownPlayerCharacter::MoveForward(float Value)
{
	LastMoveForwardValue = Value;

	if (FMath::IsNearlyZero(Value))
	{
		return;
	}

	AddMovementInput(FVector::ForwardVector, Value);
}

void ATopDownPlayerCharacter::MoveRight(float Value)
{
	LastMoveRightValue = Value;

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

int32 ATopDownPlayerCharacter::GetCurrentBombCount() const
{
	return BombInventory ? BombInventory->BombCount : 0;
}

int32 ATopDownPlayerCharacter::GetCurrentHearts() const
{
	return HeartHealth ? HeartHealth->CurrentHearts : 0;
}

void ATopDownPlayerCharacter::HandlePlaceBombPressed()
{
	if (IsValid(PlayerInteraction))
	{
		PlayerInteraction->PlaceBomb();
	}
}
