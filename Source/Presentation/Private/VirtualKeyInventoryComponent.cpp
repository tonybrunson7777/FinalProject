#include "VirtualKeyInventoryComponent.h"

UVirtualKeyInventoryComponent::UVirtualKeyInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UVirtualKeyInventoryComponent::AcquireVirtualKey(FName KeyId)
{
	if (KeyId.IsNone() || OwnedVirtualKeys.Contains(KeyId))
	{
		return false;
	}

	OwnedVirtualKeys.Add(KeyId);
	OnVirtualKeyAcquired.Broadcast(KeyId);
	return true;
}

bool UVirtualKeyInventoryComponent::ConsumeVirtualKey(FName KeyId)
{
	if (KeyId.IsNone() || !OwnedVirtualKeys.Contains(KeyId))
	{
		return false;
	}

	OwnedVirtualKeys.Remove(KeyId);
	OnVirtualKeyConsumed.Broadcast(KeyId);
	return true;
}

bool UVirtualKeyInventoryComponent::HasVirtualKey(FName KeyId) const
{
	return !KeyId.IsNone() && OwnedVirtualKeys.Contains(KeyId);
}

TArray<FName> UVirtualKeyInventoryComponent::GetOwnedVirtualKeys() const
{
	return OwnedVirtualKeys;
}
