#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Features/Item/DGItemTypes.h"
#include "DGInventoryTypes.generated.h"

class UDGItemData;

// 개체화된 아이템(장비/무기)의 고유 식별자.
// Consumable/Material/Quest 등 스택형 아이템은 개체화하지 않으므로 InstanceId를 갖지 않는다
// — HasInstanceId() == false로 스택형 여부를 구분한다.
USTRUCT(BlueprintType)
struct FDGItemInstanceId
{
	GENERATED_BODY()
public:
	// 이 Id가 실제 인스턴스를 가리키는지(장비/무기) 여부. false면 스택형 아이템을 의미한다.
	// (예전 이름 IsValid()는 "Entry 자체의 유효성"으로 오해되기 쉬워 HasInstanceId()로 명명한다.)
	bool HasInstanceId() const { return Guid.IsValid(); }

	static FDGItemInstanceId NewInstanceId()
	{
		FDGItemInstanceId Id;
		Id.Guid = FGuid::NewGuid();
		return Id;
	}

	bool operator==(const FDGItemInstanceId& Other) const { return Guid == Other.Guid; }
	bool operator!=(const FDGItemInstanceId& Other) const { return !(*this == Other); }

	friend uint32 GetTypeHash(const FDGItemInstanceId& Id) { return GetTypeHash(Id.Guid); }
	
private:
	UPROPERTY()
	FGuid Guid;
};

// Hub/Session 인벤토리 공용 잠금 상태. Hub 원본 장비가 던전에 반입되어 있는 동안 Locked로 전환된다.
UENUM(BlueprintType)
enum class EDGItemLockState : uint8
{
	Available,
	Locked
};

// Hub/Session이 공유하는 단일 아이템 보유 엔트리.
// 장비/무기(개체화 아이템)와 소모품/재료 등 스택형 아이템을 하나의 배열로 함께 관리하기 위한 통합 구조체.
//
// 규칙:
//  - 장비/무기: InstanceId.HasInstanceId() == true, Count == 1
//  - 스택형(소모품/재료/퀘스트 등): InstanceId.HasInstanceId() == false, Count >= 1
USTRUCT(BlueprintType)
struct FDGOwnedItemEntry
{
	GENERATED_BODY()

	// 장비/무기에만 유효한 Id가 채워진다. 스택형 아이템은 기본값(HasInstanceId() == false)을 유지한다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	FDGItemInstanceId InstanceId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	TSoftObjectPtr<UDGItemData> ItemData;

	// 장비/무기: 항상 1. 스택형: 1 이상.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (ClampMin = "1"))
	int32 Count = 1;

	// 개체화 아이템(장비/무기)에만 의미가 있다. 던전 반입 중이면 Locked.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	EDGItemLockState LockState = EDGItemLockState::Available;

	// LockState == Locked일 때만 유효. 어느 세션이 이 장비를 반입해 잠갔는지 식별한다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	FGuid LockingSessionId;

	// 이 Owned Item이 현재 Hub Loadout에서 선택된 슬롯. 미선택이면 None.
	// 던전 Runtime Equipment(실제 장착 상태)를 의미하지 않는다 — Hub Loadout 전용 상태다.
	// 별도의 Slot -> InstanceId 복제 상태를 두지 않고,
	// OwnedItems를 Loadout 상태의 단일 소스로 유지한다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	EDGEquipmentSlot LoadoutSlot = EDGEquipmentSlot::None;

	// Entry 자체가 유효한 데이터인지(ItemData 미설정/Count 비정상 여부)를 검사한다. InstanceId 유무와는 무관하다.
	bool IsValidEntry() const { return !ItemData.IsNull() && Count >= 1; }

	// 구조적 불변식: ItemData 로드 없이 Entry 필드만으로 판별 가능한 조합 규칙을 검사한다.
	// (Instanced ⇒ Count==1 / Stack ⇒ 미장착·Unlocked / Locked ⇒ LockingSessionId 유효)
	// ItemType 기반 게임 규칙(MatchesExpectedInstancing 등)은 별도다 — 로드된 ItemData가 필요하다.
	bool IsStructurallyConsistent() const;

	// InstanceId를 가진 개체화 아이템(장비/무기)인지.
	bool IsInstancedItem() const { return InstanceId.HasInstanceId(); }

	// InstanceId가 없는 스택형 아이템(소모품/재료/퀘스트 등)인지.
	bool IsStackItem() const { return !IsInstancedItem(); }

	// ItemData::ItemType 기준으로 개체화 규칙(Equipment/Weapon ↔ InstanceId 보유 여부)이 일치하는지 검사한다.
	// 로딩은 호출자 책임(Soft Reference 원칙) — 이미 로드된 ItemData를 전달받아 순수 조회만 수행한다.
	bool MatchesExpectedInstancing(const UDGItemData* LoadedItemData) const;
};

// 영구 재화 지갑. Tag → 보유량. 신규 재화 추가는 태그만 늘리면 되고 이 구조체는 수정하지 않는다
// (FDGStatBonus와 동일한 "태그 = 값 채널" 패턴).
USTRUCT(BlueprintType)
struct FDGCurrencyWallet
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (Categories = "Currency"))
	TMap<FGameplayTag, int64> Amounts;

	int64 GetAmount(const FGameplayTag& CurrencyTag) const
	{
		const int64* Found = Amounts.Find(CurrencyTag);
		return Found ? *Found : 0;
	}
};

// 던전 종류별 입장권 지갑. Tag → 보유 매수.
USTRUCT(BlueprintType)
struct FDGEntryTicketWallet
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (Categories = "Ticket"))
	TMap<FGameplayTag, int32> Counts;

	int32 GetCount(const FGameplayTag& TicketTag) const
	{
		const int32* Found = Counts.Find(TicketTag);
		return Found ? *Found : 0;
	}
};

// 세션 Loadout 엔트리: Hub에서 이번 던전에 반입된 개체화 장비(무기/방어구)의 스냅샷.
// Hub 전용 필드(LockState/LockingSessionId/Hub LoadoutSlot)는 포함하지 않는다 — FDGOwnedItemEntry를
// 그대로 재사용하지 않는 이유이기도 하다.
USTRUCT(BlueprintType)
struct FDGSessionLoadoutEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	FDGItemInstanceId InstanceId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	TSoftObjectPtr<UDGItemData> ItemData;

	// Hub에서 반입될 때 대상이었던 슬롯을 의미할 뿐, Dungeon Runtime Equipment(실제 장착 상태)를
	// 나타내지 않는다. Runtime Equipment는 별도 시스템에서 관리한다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	EDGEquipmentSlot EntryLoadoutSlot = EDGEquipmentSlot::None;
};

// 던전 세션 중 획득한 아이템. 장비/무기는 InstanceId를 보유하고(HasInstanceId()==true, Count==1),
// 스택형(소모품/재료 등)은 InstanceId를 보유하지 않는다(Count>=1).
USTRUCT(BlueprintType)
struct FDGSessionLootEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	FDGItemInstanceId InstanceId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	TSoftObjectPtr<UDGItemData> ItemData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (ClampMin = "1"))
	int32 Count = 1;
};
