#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HeartHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHeartsChangedSignature, int32, NewHearts, int32, MaxHearts);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathSignature);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PRESENTATION_API UHeartHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHeartHealthComponent();

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health|Hearts", meta = (ClampMin = "1", UIMin = "1"))
	int32 MaxHearts = 4;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health|Hearts", meta = (ClampMin = "0", UIMin = "0"))
	int32 StartingHearts = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health|Death")
	bool bDestroyOwnerOnDeath = false;

	UPROPERTY(BlueprintReadOnly, Category = "Health|Hearts")
	int32 CurrentHearts = 0;

	UPROPERTY(BlueprintAssignable, Category = "Health|Events")
	FOnHeartsChangedSignature OnHeartsChanged;

	UPROPERTY(BlueprintAssignable, Category = "Health|Events")
	FOnDeathSignature OnDeath;

	UFUNCTION(BlueprintCallable, Category = "Health|Hearts")
	void ApplyHeartDamage(int32 HeartsToLose);

	UFUNCTION(BlueprintCallable, Category = "Health|Hearts")
	void HealHearts(int32 HeartsToHeal);

	UFUNCTION(BlueprintPure, Category = "Health|Hearts")
	bool IsDead() const;

protected:
	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	void BroadcastHeartsChanged();
};
