#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SwitchableWallSection.generated.h"

UCLASS(Blueprintable)
class PRESENTATION_API ASwitchableWallSection : public AActor
{
	GENERATED_BODY()

public:
	ASwitchableWallSection();

	UFUNCTION(BlueprintCallable, Category = "Wall")
	void SetWallActive(bool bActive);

	UFUNCTION(BlueprintPure, Category = "Wall")
	bool IsWallActive() const { return bIsActive; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wall")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wall")
	TObjectPtr<class UStaticMeshComponent> WallMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wall")
	bool bStartsActive = true;

	UPROPERTY(BlueprintReadOnly, Category = "Wall")
	bool bIsActive = true;
};
