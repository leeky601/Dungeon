#pragma once

#include "Components/ActorComponent.h"
#include "Features/Inventory/DGInventoryTypes.h"
#include "DGSessionInventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDGOnSessionInventoryChanged);

// Dungeon 세션 동안만 존재하는 런타임 아이템 상태(PlayerState, Dungeon 서버 한정 생존 —
// SessionProgressComponent와 형제). 세션 시작 시 InitializeSessionInventory(), 종료 시
// ResetSessionInventory()로 리셋된다.
//
// Hub 영구 데이터(UDGHubStorageComponent)를 직접 참조하지 않는다 — Hub -> Session 전달은
// InitializeSessionInventory()의 인자로만 이뤄지며, 실제 Hub 조회/Persistence 연동은 이 Unit
// 범위 밖이다. 06-inventory-hub-rules.md 참고.
UCLASS(ClassGroup=(DG), meta=(BlueprintSpawnableComponent))
class DUNGEON_API UDGSessionInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDGSessionInventoryComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 던전 입장 시 Hub에서 전달된 Loadout 스냅샷으로 초기화하고 Loot을 비운다. 서버 전용.
	// EntryLoadout은 이미 구성된 배열을 호출자가 전달한다(Hub fetch/Persistence는 여기서 수행하지 않음).
	void InitializeSessionInventory(const TArray<FDGSessionLoadoutEntry>& EntryLoadout);

	// 던전 종료 시 Loadout/Loot을 모두 비운다. 서버 전용.
	void ResetSessionInventory();

	UFUNCTION(BlueprintPure, Category = "Inventory")
	const TArray<FDGSessionLoadoutEntry>& GetLoadoutItems() const { return LoadoutItems; }

	UFUNCTION(BlueprintPure, Category = "Inventory")
	const TArray<FDGSessionLootEntry>& GetLootItems() const { return LootItems; }

	// Loadout/Loot 상태가 바뀔 때 Broadcast된다. 서버 mutation 직후(서버)/OnRep(클라이언트) 양쪽에서 발생한다.
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FDGOnSessionInventoryChanged OnSessionInventoryChanged;

private:
	UFUNCTION() void OnRep_LoadoutItems();
	UFUNCTION() void OnRep_LootItems();

	// Hub에서 반입된 개체화 장비 스냅샷. Hub Lock/Loadout 관련 필드는 포함하지 않는다.
	UPROPERTY(ReplicatedUsing = OnRep_LoadoutItems)
	TArray<FDGSessionLoadoutEntry> LoadoutItems;

	// 던전 내 획득 아이템.
	UPROPERTY(ReplicatedUsing = OnRep_LootItems)
	TArray<FDGSessionLootEntry> LootItems;
};
