#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "Features/Item/DGItemTypes.h"
#include "DGItemData.generated.h"

class UTexture2D;
class UStaticMesh;

// 모든 아이템의 공통 데이터를 정의하는 최상위 DataAsset.
// UPrimaryDataAsset 기반 — AssetManager가 "Item" PrimaryAssetType으로 하위 클래스까지 일괄 스캔/로드할 수 있다.
// 신규 아이템 추가는 이 클래스(또는 하위 클래스)의 인스턴스를 만드는 것만으로 충분하며 코드 수정이 필요 없다.
UCLASS()
class DUNGEON_API UDGItemData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

	// ─── Identity ────────────────────────────────────────────────────────
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Identity", meta = (Categories = "Item"))
	FGameplayTag ItemID;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Identity")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Identity", meta = (MultiLine = true))
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Identity")
	EDGItemType ItemType = EDGItemType::None;

	// ─── Visual ──────────────────────────────────────────────────────────
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Visual")
	TSoftObjectPtr<UTexture2D> Icon;

	// 월드 드롭 표현용 — 픽업 액터가 필요한 아이템에만 설정한다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Visual")
	TSoftObjectPtr<UStaticMesh> Mesh;

	// ─── Stacking & Economy ──────────────────────────────────────────────
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Stacking", meta = (ClampMin = "1"))
	int32 MaxStackCount = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Economy", meta = (ClampMin = "0"))
	float Weight = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Economy", meta = (ClampMin = "0"))
	int32 SellPrice = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Economy", meta = (ClampMin = "0"))
	int32 BuyPrice = 0;
};