#pragma once

#include "Components/ActorComponent.h"
#include "Features/Inventory/DGInventoryTypes.h"
#include "DGHubStorageComponent.generated.h"

class UDGItemData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDGOnHubStorageChanged);

// Hub(로비) 서버에서만 생존하는 영구 보관 컴포넌트. 던전 입장 시 파괴되고, Hub 복귀 시
// Persistence에서 다시 로드된다 — 던전 세션 동안 실시간으로 참조 가능한 살아있는 인스턴스는 없다.
// 자세한 서버 토폴로지/Lock 메커니즘/Settlement 흐름은 06-inventory-hub-rules.md 참고.
//
// 현재는 필드/조회 스캐폴딩 단계다. 장착/판매/던전 입장 요청 같은 쓰기 경로(Server RPC 핸들러)는
// PlayerController가 소유하며(02-networking-authority.md의 "Hub vs Session RPC 분리" 참고),
// Persistence 인터페이스 확정 이후 별도로 연결한다.
UCLASS(ClassGroup=(DG), meta=(BlueprintSpawnableComponent))
class DUNGEON_API UDGHubStorageComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDGHubStorageComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 장비/무기/스택형 아이템을 통틀어 관리하는 단일 배열. FDGOwnedItemEntry::IsInstancedItem() / IsStackItem()로 구분한다.
	UFUNCTION(BlueprintPure, Category = "Inventory")
	const TArray<FDGOwnedItemEntry>& GetOwnedItems() const { return OwnedItems; }

	// InstanceId로 개체화 아이템(장비/무기) 엔트리를 조회한다. 스택형 아이템에는 사용할 수 없다.
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool FindEntryByInstanceId(FDGItemInstanceId InstanceId, FDGOwnedItemEntry& OutEntry) const;

	// 지정한 슬롯이 현재 Hub Loadout에서 선택된 엔트리를 조회한다(Runtime Equipment 아님). 없으면 false.
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool GetLoadoutItemInSlot(EDGEquipmentSlot Slot, FDGOwnedItemEntry& OutEntry) const;

	// 현재 Hub Loadout(OwnedItems 중 LoadoutSlot != None)을 Dungeon 반입용 Snapshot으로 변환한다.
	// 순수 조회 함수 — LoadSynchronous를 호출하지 않으며, ItemData가 로드되지 않은 Entry는 LoadoutSlot을
	// 그대로 신뢰해 포함한다(로드돼 있으면 EquipmentData::EquipmentSlot과 일치하는지만 추가 검증한다).
	// Stack Item이 LoadoutSlot을 가진 상태, 구조적으로 불일치한 Entry, 중복 InstanceId/Slot을 만나면
	// 전체 실패(false)하고 OutLoadout은 건드리지 않는다. LockState는 검사하지 않는다 — 이번 Unit에서는
	// Lock 트랜잭션(TryBeginDungeonEntry)을 다루지 않는다.
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool BuildSessionLoadout(TArray<FDGSessionLoadoutEntry>& OutLoadout) const;

	// 서버 권한 전용. 구조적으로 유효한(FDGOwnedItemEntry::IsStructurallyConsistent()) Entry만 추가한다.
	// Instanced Item은 새 Entry로 추가되며, 동일 InstanceId가 이미 존재하면 거부한다.
	// Stack Item은 동일 ItemData Entry가 있으면 Count를 합산하고, 없으면 새 Entry로 추가한다.
	// MaxStackCount는 ItemData가 이미 로드되어 있을 때만 검증한다(강제 동기 로드하지 않음 — 로딩은 호출자 책임).
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddOwnedItem(const FDGOwnedItemEntry& NewEntry);

	// 서버 권한 전용. InstanceId로 개체화 아이템 하나를 제거한다. Stack Item 수량 차감은 다루지 않는다.
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveOwnedItem(FDGItemInstanceId InstanceId);

	// 서버 권한 전용. 지정한 ItemData의 Stack Item 수량을 Count만큼 차감한다.
	// 보유량보다 많이 요청하면 부분 차감 없이 전체 실패한다. 차감 후 Count가 0이 되면 Entry를 제거한다.
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveStackItem(TSoftObjectPtr<UDGItemData> ItemData, int32 Count);

	// 서버 권한 전용. Hub Loadout 아이템 선택(Dungeon Runtime Equipment와는 별개 — 06-inventory-hub-rules.md 참고).
	// Instanced Item만 대상이며, ItemData가 이미 로드되어 있어야 한다(강제 로드하지 않음). 목표 Slot은
	// 호출자가 전달하지 않고 서버가 UDGEquipmentData::EquipmentSlot을 직접 읽어 사용한다(EquipmentSlot이
	// None이면 실패). Locked Entry는 실패한다. 동일 Slot을 이미 사용 중인 Entry가 있으면 자동 교체하되,
	// 그 Entry가 Locked면 전체 요청을 실패시키고 아무 상태도 바꾸지 않는다. 이미 같은 Slot이면 상태
	// 변경 없이 성공(Broadcast 없음).
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool SetLoadoutItem(FDGItemInstanceId InstanceId);

	// 서버 권한 전용. Hub Loadout 아이템 선택 해제. Locked Entry는 실패한다.
	// 이미 LoadoutSlot == None이면 상태 변경 없이 성공(Broadcast 없음).
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool ClearLoadoutItem(FDGItemInstanceId InstanceId);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	int64 GetCurrencyAmount(FGameplayTag CurrencyTag) const { return Currency.GetAmount(CurrencyTag); }

	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetTicketCount(FGameplayTag TicketTag) const { return Tickets.GetCount(TicketTag); }

	// Hub 보관 상태(OwnedItems)가 바뀔 때 Broadcast된다. 서버 mutation 직후(서버)/OnRep(클라이언트) 양쪽에서 발생한다.
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FDGOnHubStorageChanged OnHubStorageChanged;

	// OwnedItems OwnerOnly replication PIE 검증 전용. 에디터(Blueprint 디폴트)에서 유효한 UDGItemData
	// 애셋을 직접 지정해야 한다 — 코드에서 임의로 애셋을 만들거나 경로를 하드코딩하지 않는다.
	UPROPERTY(EditDefaultsOnly, Category = "Inventory|Debug")
	TSoftObjectPtr<UDGItemData> Debug_TestItemData;

	// 디버그 전용: 서버에서 Debug_TestItemData로 구조적으로 유효한 Stack Entry를 AddOwnedItem()을 통해 추가한다.
	// Count를 지정해 Stack 병합/MaxStackCount 초과 거부 동작을 함께 검증할 수 있다.
	// 서버 권한이 없으면 무시하고 경고 로그만 남긴다(Debug_AddSessionXP와 동일 패턴). Production Mutation API(AddOwnedItem)는 그대로 사용한다.
	UFUNCTION(BlueprintCallable, Category = "Inventory|Debug")
	void Debug_AddTestOwnedItem(int32 Count = 1);

	// 디버그 전용: Debug_TestItemData(Equipment/Weapon 애셋을 지정해야 함)로 새 InstanceId를 발급해
	// Instanced Entry를 AddOwnedItem()을 통해 추가한다. SetLoadoutItem/ClearLoadoutItem PIE 검증용.
	// 서버 권한이 없거나 추가에 실패하면 무효 Id(HasInstanceId()==false)를 반환한다.
	UFUNCTION(BlueprintCallable, Category = "Inventory|Debug")
	FDGItemInstanceId Debug_AddTestInstancedItem();

private:
	UFUNCTION() void OnRep_OwnedItems();

	// InstanceId로 OwnedItems 내 index를 찾는다. 없으면 INDEX_NONE.
	int32 IndexOfByInstanceId(FDGItemInstanceId InstanceId) const;

	// 동일 ItemData를 가진 Stack Item(IsStackItem()==true) Entry의 index를 찾는다. 없으면 INDEX_NONE.
	int32 IndexOfStackByItemData(const TSoftObjectPtr<UDGItemData>& ItemData) const;

	// 지정한 Slot을 현재 LoadoutSlot으로 가진 OwnedItems 내 index를 찾는다. 없으면 INDEX_NONE.
	int32 IndexOfLoadoutSlot(EDGEquipmentSlot Slot) const;

	// 장비/무기/스택형 아이템 전체. Lock 메커니즘·장착 슬롯 정보는 각 Entry가 스스로 보관한다.
	UPROPERTY(ReplicatedUsing = OnRep_OwnedItems)
	TArray<FDGOwnedItemEntry> OwnedItems;

	// TODO: TMap은 UE 기본 프로퍼티 복제를 지원하지 않는다. Persistence 연동 시점에 필요하면
	// FFastArraySerializer 기반 구조로 전환하거나, 클라이언트 요청 RPC 응답으로 대체한다.
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FDGCurrencyWallet Currency;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FDGEntryTicketWallet Tickets;
};
