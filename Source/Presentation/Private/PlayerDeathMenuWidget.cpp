#include "PlayerDeathMenuWidget.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

void UPlayerDeathMenuWidget::InitializeDeathMenu(APlayerController* OwningController)
{
	CachedPlayerController = OwningController;
}

void UPlayerDeathMenuWidget::PrepareForLevelTravel()
{
	APlayerController* PC = CachedPlayerController.Get();
	if (!IsValid(PC))
	{
		PC = GetOwningPlayer();
	}

	if (IsValid(PC))
	{
		if (UWorld* World = PC->GetWorld())
		{
			UGameplayStatics::SetGamePaused(World, false);
		}

		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = false;
		PC->SetIgnoreMoveInput(false);
		PC->SetIgnoreLookInput(false);

		if (APawn* Pawn = PC->GetPawn())
		{
			Pawn->EnableInput(PC);
		}
	}

	RemoveFromParent();
}

void UPlayerDeathMenuWidget::GoToMainMenu()
{
	APlayerController* PC = CachedPlayerController.Get();
	if (!IsValid(PC))
	{
		PC = GetOwningPlayer();
	}

	UWorld* World = IsValid(PC) ? PC->GetWorld() : GetWorld();
	if (!IsValid(World) || MainMenuLevelName.IsNone())
	{
		return;
	}

	PrepareForLevelTravel();
	UGameplayStatics::OpenLevel(World, MainMenuLevelName);
}

void UPlayerDeathMenuWidget::RestartLevel()
{
	APlayerController* PC = CachedPlayerController.Get();
	if (!IsValid(PC))
	{
		PC = GetOwningPlayer();
	}

	UWorld* World = IsValid(PC) ? PC->GetWorld() : GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	FName LevelToOpen = RestartLevelName;
	if (LevelToOpen.IsNone())
	{
		LevelToOpen = FName(*UGameplayStatics::GetCurrentLevelName(World, true));
	}

	if (LevelToOpen.IsNone())
	{
		return;
	}

	PrepareForLevelTravel();
	UGameplayStatics::OpenLevel(World, LevelToOpen);
}
