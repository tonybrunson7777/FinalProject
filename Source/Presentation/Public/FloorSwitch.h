#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloorSwitch.generated.h"

class ASwitchableWallSection;
class UBoxComponent;
class UPrimitiveComponent;
struct FHitResult;

UCLASS(Blueprintable)
class PRESENTATION_API AFloorSwitch : public AActor
{
	GENERATED_BODY()

public:
	AFloorSwitch();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Switch")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Switch")
	TObjectPtr<UBoxComponent> TriggerBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Switch")
	TObjectPtr<class UStaticMeshComponent> SwitchMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Switch")
	TArray<TObjectPtr<ASwitchableWallSection>> LinkedWallSections;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Switch")
	bool bStartsActivated = false;

	UPROPERTY(BlueprintReadOnly, Category = "Switch")
	bool bIsActivated = false;

	UFUNCTION()
	void HandleOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void ApplySwitchStateToWalls();
	void ToggleSwitch();
};
