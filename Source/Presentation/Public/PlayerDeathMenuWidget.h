#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerDeathMenuWidget.generated.h"

UCLASS(Blueprintable)
class PRESENTATION_API UPlayerDeathMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Level to open when the player chooses Main Menu (e.g. presentationMap). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Death|Levels")
	FName MainMenuLevelName = FName(TEXT("presentationMap"));

	/** Level to reload for Play Again. If None, the current level is restarted. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Death|Levels")
	FName RestartLevelName = NAME_None;

	UFUNCTION(BlueprintCallable, Category = "Death")
	void InitializeDeathMenu(class APlayerController* OwningController);

	/** Wire this to the Main Menu button OnClicked event in your death menu widget BP. */
	UFUNCTION(BlueprintCallable, Category = "Death")
	void GoToMainMenu();

	/** Wire this to the Play Again button OnClicked event in your death menu widget BP. */
	UFUNCTION(BlueprintCallable, Category = "Death")
	void RestartLevel();

	void PrepareForLevelTravel();

protected:
	UPROPERTY()
	TWeakObjectPtr<APlayerController> CachedPlayerController;
};
