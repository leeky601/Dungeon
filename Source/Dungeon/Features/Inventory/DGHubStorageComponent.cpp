#include "Features/Inventory/DGHubStorageComponent.h"
#include "Net/UnrealNetwork.h"
#include "Dungeon.h"
#include "Features/Item/Data/DGItemData.h"
#include "Features/Item/Data/DGEquipmentData.h"

UDGHubStorageComponent::UDGHubStorageComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UDGHubStorageComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UDGHubStorageComponent, OwnedItems, COND_OwnerOnly, REPNOTIFY_OnChanged);
}

bool UDGHubStorageComponent::FindEntryByInstanceId(FDGItemInstanceId InstanceId, FDGOwnedItemEntry& OutEntry) const
{
	if (!InstanceId.HasInstanceId())
	{
		return false;
	}

	for (const FDGOwnedItemEntry& Entry : OwnedItems)
	{
		if (Entry.InstanceId == InstanceId)
		{
			OutEntry = Entry;
			return true;
		}
	}

	return false;
}

bool UDGHubStorageComponent::GetLoadoutItemInSlot(EDGEquipmentSlot Slot, FDGOwnedItemEntry& OutEntry) const
{
	if (Slot == EDGEquipmentSlot::None)
	{
		return false;
	}

	for (const FDGOwnedItemEntry& Entry : OwnedItems)
	{
		if (Entry.LoadoutSlot == Slot)
		{
			OutEntry = Entry;
			return true;
		}
	}

	return false;
}

bool UDGHubStorageComponent::BuildSessionLoadout(TArray<FDGSessionLoadoutEntry>& OutLoadout) const
{
	TArray<FDGSessionLoadoutEntry> Result;

	for (const FDGOwnedItemEntry& Entry : OwnedItems)
	{
		if (Entry.LoadoutSlot == EDGEquipmentSlot::None)
		{
			// 미선택 — Snapshot 대상 아님(Stack Item도 정상 상태라면 여기서 걸러진다).
			continue;
		}

		if (Entry.IsStackItem() || !Entry.IsStructurallyConsistent())
		{
			UE_LOG(LogDungeon, Warning, TEXT("[Inventory] BuildSessionLoadout: rejected structurally inconsistent loadout entry on %s"), *GetNameSafe(GetOwner()));
			return false;
		}

		// ItemData가 이미 로드되어 있을 때만 LoadoutSlot과 실제 EquipmentSlot 일치 여부를 재검증한다
		// (강제 로드하지 않음 — AddOwnedItem의 "로드된 경우에만 추가 검증" 정책과 동일).
		if (const UDGItemData* LoadedItemData = Entry.ItemData.Get())
		{
			const UDGEquipmentData* EquipmentData = Cast<UDGEquipmentData>(LoadedItemData);
			if (!EquipmentData || EquipmentData->EquipmentSlot != Entry.LoadoutSlot)
			{
				UE_LOG(LogDungeon, Warning, TEXT("[Inventory] BuildSessionLoadout: rejected entry whose LoadoutSlot doesn't match loaded EquipmentData on %s"), *GetNameSafe(GetOwner()));
				return false;
			}
		}

		if (Result.ContainsByPredicate([&Entry](const FDGSessionLoadoutEntry& Existing) { return Existing.InstanceId == Entry.InstanceId; }))
		{
			UE_LOG(LogDungeon, Warning, TEXT("[Inventory] BuildSessionLoadout: rejected duplicate InstanceId on %s"), *GetNameSafe(GetOwner()));
			return false;
		}

		if (Result.ContainsByPredicate([&Entry](const FDGSessionLoadoutEntry& Existing) { return Existing.EntryLoadoutSlot == Entry.LoadoutSlot; }))
		{
			UE_LOG(LogDungeon, Warning, TEXT("[Inventory] BuildSessionLoadout: rejected duplicate EquipmentSlot on %s"), *GetNameSafe(GetOwner()));
			return false;
		}

		FDGSessionLoadoutEntry SessionEntry;
		SessionEntry.InstanceId = Entry.InstanceId;
		SessionEntry.ItemData = Entry.ItemData;
		SessionEntry.EntryLoadoutSlot = Entry.LoadoutSlot;
		Result.Add(SessionEntry);
	}

	OutLoadout = MoveTemp(Result);
	return true;
}

bool UDGHubStorageComponent::AddOwnedItem(const FDGOwnedItemEntry& NewEntry)
{
	AActor* Owner = GetOwner();
	if (!Owner || !Owner->HasAuthority())
	{
		UE_LOG(LogDungeon, Warning, TEXT("[Inventory] AddOwnedItem: called without authority on %s"), *GetNameSafe(Owner));
		return false;
	}

	if (!NewEntry.IsStructurallyConsistent())
	{
		UE_LOG(LogDungeon, Warning, TEXT("[Inventory] AddOwnedItem: rejected structurally inconsistent entry on %s"), *Owner->GetName());
		return false;
	}

	// ItemData가 이미 로드되어 있을 때만 ItemType 기준 개체화 규칙을 검증한다(강제 로드하지 않음 —
	// MaxStackCount 검증과 동일한 "로드되어 있을 때만" 정책).
	if (const UDGItemData* LoadedItemData = NewEntry.ItemData.Get())
	{
		if (!NewEntry.MatchesExpectedInstancing(LoadedItemData))
		{
			UE_LOG(LogDungeon, Warning, TEXT("[Inventory] AddOwnedItem: rejected entry not matching expected instancing rule (ItemType vs InstanceId) on %s"), *Owner->GetName());
			return false;
		}
	}

	if (NewEntry.IsInstancedItem())
	{
		if (IndexOfByInstanceId(NewEntry.InstanceId) != INDEX_NONE)
		{
			UE_LOG(LogDungeon, Warning, TEXT("[Inventory] AddOwnedItem: rejected duplicate InstanceId on %s"), *Owner->GetName());
			return false;
		}

		OwnedItems.Add(NewEntry);
		OnHubStorageChanged.Broadcast();
		return true;
	}

	// Stack Item: 강제 동기 로드하지 않는다 — 이미 로드되어 있을 때만 MaxStackCount를 검증한다(로딩은 호출자 책임).
	int32 MaxStackCount = TNumericLimits<int32>::Max();
	if (const UDGItemData* LoadedItemData = NewEntry.ItemData.Get())
	{
		MaxStackCount = LoadedItemData->MaxStackCount;
	}

	const int32 ExistingIndex = IndexOfStackByItemData(NewEntry.ItemData);
	if (ExistingIndex != INDEX_NONE)
	{
		FDGOwnedItemEntry& Existing = OwnedItems[ExistingIndex];
		if (Existing.Count + NewEntry.Count > MaxStackCount)
		{
			UE_LOG(LogDungeon, Warning, TEXT("[Inventory] AddOwnedItem: rejected stack merge exceeding MaxStackCount (%d + %d > %d) on %s"),
				Existing.Count, NewEntry.Count, MaxStackCount, *Owner->GetName());
			return false;
		}

		Existing.Count += NewEntry.Count;
	}
	else
	{
		if (NewEntry.Count > MaxStackCount)
		{
			UE_LOG(LogDungeon, Warning, TEXT("[Inventory] AddOwnedItem: rejected new stack exceeding MaxStackCount (%d > %d) on %s"),
				NewEntry.Count, MaxStackCount, *Owner->GetName());
			return false;
		}

		OwnedItems.Add(NewEntry);
	}

	OnHubStorageChanged.Broadcast();
	return true;
}

bool UDGHubStorageComponent::RemoveOwnedItem(FDGItemInstanceId InstanceId)
{
	AActor* Owner = GetOwner();
	if (!Owner || !Owner->HasAuthority())
	{
		UE_LOG(LogDungeon, Warning, TEXT("[Inventory] RemoveOwnedItem: called without authority on %s"), *GetNameSafe(Owner));
		return false;
	}

	if (!InstanceId.HasInstanceId())
	{
		UE_LOG(LogDungeon, Warning, TEXT("[Inventory] RemoveOwnedItem: invalid InstanceId on %s"), *Owner->GetName());
		return false;
	}

	const int32 Index = IndexOfByInstanceId(InstanceId);
	if (Index == INDEX_NONE)
	{
		UE_LOG(LogDungeon, Warning, TEXT("[Inventory] RemoveOwnedItem: entry not found on %s"), *Owner->GetName());
		return false;
	}

	OwnedItems.RemoveAt(Index);
	OnHubStorageChanged.Broadcast();
	return true;
}

bool UDGHubStorageComponent::RemoveStackItem(TSoftObjectPtr<UDGItemData> ItemData, int32 Count)
{
	AActor* Owner = GetOwner();
	if (!Owner || !Owner->HasAuthority())
	{
		UE_LOG(LogDungeon, Warning, TEXT("[Inventory] RemoveStackItem: called without authority on %s"), *GetNameSafe(Owner));
		return false;
	}

	if (Count <= 0)
	{
		UE_LOG(LogDungeon, Warning, TEXT("[Inventory] RemoveStackItem: invalid Count (%d) on %s"), Count, *Owner->GetName());
		return false;
	}

	const int32 Index = IndexOfStackByItemData(ItemData);
	if (Index == INDEX_NONE)
	{
		UE_LOG(LogDungeon, Warning, TEXT("[Inventory] RemoveStackItem: stack not found on %s"), *Owner->GetName());
		return false;
	}

	FDGOwnedItemEntry& Existing = OwnedItems[Index];
	if (Existing.Count < Count)
	{
		UE_LOG(LogDungeon, Warning, TEXT("[Inventory] RemoveStackItem: requested Count (%d) exceeds owned Count (%d) on %s"),
			Count, Existing.Count, *Owner->GetName());
		return false;
	}

	Existing.Count -= Count;
	if (Existing.Count == 0)
	{
		OwnedItems.RemoveAt(Index);
	}

	OnHubStorageChanged.Broadcast();
	return true;
}

bool UDGHubStorageComponent::SetLoadoutItem(FDGItemInstanceId InstanceId)
{
	AActor* Owner = GetOwner();
	if (!Owner || !Owner->HasAuthority())
	{
		UE_LOG(LogDungeon, Warning, TEXT("[Inventory] SetLoadoutItem: called without authority on %s"), *GetNameSafe(Owner));
		return false;
	}

	if (!InstanceId.HasInstanceId())
	{
		UE_LOG(LogDungeon, Warning, TEXT("[Inventory] SetLoadoutItem: invalid InstanceId on %s"), *Owner->GetName());
		return false;
	}

	const int32 EntryIndex = IndexOfByInstanceId(InstanceId);
	if (EntryIndex == INDEX_NONE)
	{
		UE_LOG(LogDungeon, Warning, TEXT("[Inventory] SetLoadoutItem: entry not found on %s"), *Owner->GetName());
		return false;
	}

	FDGOwnedItemEntry& Entry = OwnedItems[EntryIndex];
	if (!Entry.IsInstancedItem())
	{
		UE_LOG(LogDungeon, Warning, TEXT("[Inventory] SetLoadoutItem: entry is not an instanced item on %s"), *Owner->GetName());
		return false;
	}

	if (Entry.LockState == EDGItemLockState::Locked)
	{
		UE_LOG(LogDungeon, Warning, TEXT("[Inventory] SetLoadoutItem: entry is locked on %s"), *Owner->GetName());
		return false;
	}

	const UDGItemData* LoadedItemData = Entry.ItemData.Get();
	if (!LoadedItemData)
	{
		UE_LOG(LogDungeon, Warning, TEXT("[Inventory] SetLoadoutItem: ItemData not loaded on %s"), *Owner->GetName());
		return false;
	}

	const UDGEquipmentData* EquipmentData = Cast<UDGEquipmentData>(LoadedItemData);
	if (!EquipmentData || EquipmentData->EquipmentSlot == EDGEquipmentSlot::None)
	{
		UE_LOG(LogDungeon, Warning, TEXT("[Inventory] SetLoadoutItem: ItemData is not equippable on %s"), *Owner->GetName());
		return false;
	}

	const EDGEquipmentSlot Slot = EquipmentData->EquipmentSlot;

	if (Entry.LoadoutSlot == Slot)
	{
		// 이미 원하는 최종 상태 — no-op 성공, Broadcast 생략.
		return true;
	}

	const int32 PreviousIndex = IndexOfLoadoutSlot(Slot);
	if (PreviousIndex != INDEX_NONE && OwnedItems[PreviousIndex].LockState == EDGItemLockState::Locked)
	{
		UE_LOG(LogDungeon, Warning, TEXT("[Inventory] SetLoadoutItem: existing slot occupant is locked on %s"), *Owner->GetName());
		return false;
	}

	if (PreviousIndex != INDEX_NONE)
	{
		OwnedItems[PreviousIndex].LoadoutSlot = EDGEquipmentSlot::None;
	}

	Entry.LoadoutSlot = Slot;
	OnHubStorageChanged.Broadcast();
	return true;
}

bool UDGHubStorageComponent::ClearLoadoutItem(FDGItemInstanceId InstanceId)
{
	AActor* Owner = GetOwner();
	if (!Owner || !Owner->HasAuthority())
	{
		UE_LOG(LogDungeon, Warning, TEXT("[Inventory] ClearLoadoutItem: called without authority on %s"), *GetNameSafe(Owner));
		return false;
	}

	if (!InstanceId.HasInstanceId())
	{
		UE_LOG(LogDungeon, Warning, TEXT("[Inventory] ClearLoadoutItem: invalid InstanceId on %s"), *Owner->GetName());
		return false;
	}

	const int32 EntryIndex = IndexOfByInstanceId(InstanceId);
	if (EntryIndex == INDEX_NONE)
	{
		UE_LOG(LogDungeon, Warning, TEXT("[Inventory] ClearLoadoutItem: entry not found on %s"), *Owner->GetName());
		return false;
	}

	FDGOwnedItemEntry& Entry = OwnedItems[EntryIndex];
	if (!Entry.IsInstancedItem())
	{
		UE_LOG(LogDungeon, Warning, TEXT("[Inventory] ClearLoadoutItem: entry is not an instanced item on %s"), *Owner->GetName());
		return false;
	}

	if (Entry.LockState == EDGItemLockState::Locked)
	{
		UE_LOG(LogDungeon, Warning, TEXT("[Inventory] ClearLoadoutItem: entry is locked on %s"), *Owner->GetName());
		return false;
	}

	if (Entry.LoadoutSlot == EDGEquipmentSlot::None)
	{
		// 이미 원하는 최종 상태 — no-op 성공, Broadcast 생략.
		return true;
	}

	Entry.LoadoutSlot = EDGEquipmentSlot::None;
	OnHubStorageChanged.Broadcast();
	return true;
}

int32 UDGHubStorageComponent::IndexOfByInstanceId(FDGItemInstanceId InstanceId) const
{
	return OwnedItems.IndexOfByPredicate([InstanceId](const FDGOwnedItemEntry& Entry)
	{
		return Entry.InstanceId == InstanceId;
	});
}

int32 UDGHubStorageComponent::IndexOfStackByItemData(const TSoftObjectPtr<UDGItemData>& ItemData) const
{
	return OwnedItems.IndexOfByPredicate([&ItemData](const FDGOwnedItemEntry& Entry)
	{
		return Entry.IsStackItem() && Entry.ItemData == ItemData;
	});
}

int32 UDGHubStorageComponent::IndexOfLoadoutSlot(EDGEquipmentSlot Slot) const
{
	return OwnedItems.IndexOfByPredicate([Slot](const FDGOwnedItemEntry& Entry)
	{
		return Entry.LoadoutSlot == Slot;
	});
}

void UDGHubStorageComponent::Debug_AddTestOwnedItem(int32 Count)
{
	AActor* Owner = GetOwner();
	if (!Owner || !Owner->HasAuthority())
	{
		UE_LOG(LogDungeon, Warning, TEXT("[Inventory][Debug][Server] Debug_AddTestOwnedItem: called without authority on %s"), *GetNameSafe(Owner));
		return;
	}

	if (Debug_TestItemData.IsNull())
	{
		UE_LOG(LogDungeon, Warning, TEXT("[Inventory][Debug][Server] Debug_AddTestOwnedItem: Debug_TestItemData not set on %s. Assign a valid UDGItemData asset on the ADGHubPlayerState Blueprint defaults."), *Owner->GetName());
		return;
	}

	if (Count <= 0)
	{
		UE_LOG(LogDungeon, Warning, TEXT("[Inventory][Debug][Server] Debug_AddTestOwnedItem: invalid Count (%d) on %s"), Count, *Owner->GetName());
		return;
	}

	FDGOwnedItemEntry TestEntry;
	TestEntry.ItemData = Debug_TestItemData;
	TestEntry.Count = Count;

	const bool bAdded = AddOwnedItem(TestEntry);

	const int32 StackIndex = IndexOfStackByItemData(Debug_TestItemData);
	const int32 ResultStackCount = StackIndex != INDEX_NONE ? OwnedItems[StackIndex].Count : 0;

	UE_LOG(LogDungeon, Log, TEXT("[Inventory][Debug][Server] Debug_AddTestOwnedItem: PlayerState=%s RequestedCount=%d bAdded=%d StackCount=%d OwnedItemsCount=%d"),
		*Owner->GetName(), Count, bAdded, ResultStackCount, OwnedItems.Num());
}

FDGItemInstanceId UDGHubStorageComponent::Debug_AddTestInstancedItem()
{
	AActor* Owner = GetOwner();
	if (!Owner || !Owner->HasAuthority())
	{
		UE_LOG(LogDungeon, Warning, TEXT("[Inventory][Debug][Server] Debug_AddTestInstancedItem: called without authority on %s"), *GetNameSafe(Owner));
		return FDGItemInstanceId();
	}

	if (Debug_TestItemData.IsNull())
	{
		UE_LOG(LogDungeon, Warning, TEXT("[Inventory][Debug][Server] Debug_AddTestInstancedItem: Debug_TestItemData not set on %s. Assign a valid Equipment/Weapon UDGItemData asset on the ADGHubPlayerState Blueprint defaults."), *Owner->GetName());
		return FDGItemInstanceId();
	}

	// 디버그 전용 예외: SetLoadoutItem 검증(ItemData가 이미 로드돼 있어야 함)을 실제로 통과시키기 위해
	// 이 호출자(Debug 함수)가 PIE 테스트 fixture 준비 목적으로 명시적으로 동기 로드한다.
	// Production Mutation 경로(SetLoadoutItem 등)는 강제 로드하지 않는다 — 이 예외를 확장하지 않는다.
	if (!Debug_TestItemData.LoadSynchronous())
	{
		UE_LOG(LogDungeon, Warning, TEXT("[Inventory][Debug][Server] Debug_AddTestInstancedItem: failed to load Debug_TestItemData on %s."), *Owner->GetName());
		return FDGItemInstanceId();
	}

	FDGOwnedItemEntry TestEntry;
	TestEntry.InstanceId = FDGItemInstanceId::NewInstanceId();
	TestEntry.ItemData = Debug_TestItemData;
	TestEntry.Count = 1;

	const bool bAdded = AddOwnedItem(TestEntry);

	UE_LOG(LogDungeon, Log, TEXT("[Inventory][Debug][Server] Debug_AddTestInstancedItem: PlayerState=%s bAdded=%d OwnedItemsCount=%d"),
		*Owner->GetName(), bAdded, OwnedItems.Num());

	return bAdded ? TestEntry.InstanceId : FDGItemInstanceId();
}

void UDGHubStorageComponent::OnRep_OwnedItems()
{
	UE_LOG(LogDungeon, Log, TEXT("[Inventory][Debug][Client] OnRep_OwnedItems: PlayerState=%s OwnedItemsCount=%d"),
		*GetNameSafe(GetOwner()), OwnedItems.Num());

	OnHubStorageChanged.Broadcast();
}
