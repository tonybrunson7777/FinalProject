#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VirtualKeyInventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVirtualKeyChangedSignature, FName, KeyId);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PRESENTATION_API UVirtualKeyInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UVirtualKeyInventoryComponent();

	UPROPERTY(BlueprintAssignable, Category = "Keys|Events")
	FOnVirtualKeyChangedSignature OnVirtualKeyAcquired;

	UPROPERTY(BlueprintAssignable, Category = "Keys|Events")
	FOnVirtualKeyChangedSignature OnVirtualKeyConsumed;

	UFUNCTION(BlueprintCallable, Category = "Keys")
	bool AcquireVirtualKey(FName KeyId);

	UFUNCTION(BlueprintCallable, Category = "Keys")
	bool ConsumeVirtualKey(FName KeyId);

	UFUNCTION(BlueprintPure, Category = "Keys")
	bool HasVirtualKey(FName KeyId) const;

	UFUNCTION(BlueprintPure, Category = "Keys")
	TArray<FName> GetOwnedVirtualKeys() const;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Keys")
	TArray<FName> OwnedVirtualKeys;
};
