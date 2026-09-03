#include "Features/Inventory/DGSessionInventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "Dungeon.h"

UDGSessionInventoryComponent::UDGSessionInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UDGSessionInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UDGSessionInventoryComponent, LoadoutItems, COND_OwnerOnly, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(UDGSessionInventoryComponent, LootItems,    COND_OwnerOnly, REPNOTIFY_OnChanged);
}

void UDGSessionInventoryComponent::InitializeSessionInventory(const TArray<FDGSessionLoadoutEntry>& EntryLoadout)
{
	AActor* Owner = GetOwner();
	if (!Owner || !Owner->HasAuthority())
	{
		UE_LOG(LogDungeon, Warning, TEXT("[Inventory] InitializeSessionInventory: called without authority on %s"), *GetNameSafe(Owner));
		return;
	}

	LoadoutItems = EntryLoadout;
	LootItems.Reset();

	UE_LOG(LogDungeon, Log, TEXT("[Inventory] InitializeSessionInventory: %d Loadout item(s) Owner=%s"),
		LoadoutItems.Num(), *Owner->GetName());

	OnSessionInventoryChanged.Broadcast();
}

void UDGSessionInventoryComponent::ResetSessionInventory()
{
	AActor* Owner = GetOwner();
	if (!Owner || !Owner->HasAuthority())
	{
		UE_LOG(LogDungeon, Warning, TEXT("[Inventory] ResetSessionInventory: called without authority on %s"), *GetNameSafe(Owner));
		return;
	}

	LoadoutItems.Reset();
	LootItems.Reset();

	UE_LOG(LogDungeon, Log, TEXT("[Inventory] ResetSessionInventory: Owner=%s"), *Owner->GetName());

	OnSessionInventoryChanged.Broadcast();
}

void UDGSessionInventoryComponent::OnRep_LoadoutItems()
{
	OnSessionInventoryChanged.Broadcast();
}

void UDGSessionInventoryComponent::OnRep_LootItems()
{
	OnSessionInventoryChanged.Broadcast();
}
