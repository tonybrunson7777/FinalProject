#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CrackedWall.generated.h"

UCLASS(Blueprintable)
class PRESENTATION_API ACrackedWall : public AActor
{
	GENERATED_BODY()

public:
	ACrackedWall();

protected:
	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wall")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wall")
	TObjectPtr<class UStaticMeshComponent> WallMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wall|Health")
	TObjectPtr<class UHeartHealthComponent> HeartHealth;

	UFUNCTION()
	void HandleWallDestroyed();
};
