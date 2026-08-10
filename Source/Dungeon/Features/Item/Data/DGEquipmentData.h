#pragma once

#include "Features/Item/Data/DGItemData.h"
#include "Features/Item/DGItemTypes.h"
#include "DGEquipmentData.generated.h"

// 장착 가능한 아이템의 공통 데이터. ItemData를 상속해 장착 슬롯 / 스탯 보너스를 확장한다.
UCLASS()
class DUNGEON_API UDGEquipmentData : public UDGItemData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	EDGEquipmentSlot EquipmentSlot = EDGEquipmentSlot::None;

	// 장비 등급 — 기본 스탯(StatBonus) 수치에는 영향을 주지 않는다.
	// 등급이 높을수록 UDGEquipmentSkillTreeData에서 선택 가능한 노드/빌드 확장성이 넓어지는 것으로만 차이를 표현한다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	EDGItemRarity Rarity = EDGItemRarity::Common;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	FDGStatBonus StatBonus;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment", meta = (ClampMin = "1"))
	int32 RequiredLevel = 1;
};