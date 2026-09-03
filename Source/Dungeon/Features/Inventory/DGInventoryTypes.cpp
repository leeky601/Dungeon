#include "Features/Inventory/DGInventoryTypes.h"
#include "Features/Item/Data/DGItemData.h"

bool FDGOwnedItemEntry::IsStructurallyConsistent() const
{
	if (!IsValidEntry())
	{
		return false;
	}

	if (IsInstancedItem())
	{
		if (Count != 1)
		{
			return false;
		}

		if (LockState == EDGItemLockState::Locked && !LockingSessionId.IsValid())
		{
			return false;
		}
	}
	else
	{
		// Stack Item: Loadout 선택 불가, Lock 불가 (LockState/LockingSessionId는 개체화 아이템에만 의미가 있다).
		if (LoadoutSlot != EDGEquipmentSlot::None)
		{
			return false;
		}

		if (LockState != EDGItemLockState::Available || LockingSessionId.IsValid())
		{
			return false;
		}
	}

	return true;
}

bool FDGOwnedItemEntry::MatchesExpectedInstancing(const UDGItemData* LoadedItemData) const
{
	if (!LoadedItemData)
	{
		return false;
	}

	switch (LoadedItemData->ItemType)
	{
	case EDGItemType::Equipment:
	case EDGItemType::Weapon:
		return IsInstancedItem() && Count == 1;
	default:
		return IsStackItem() && Count >= 1;
	}
}
