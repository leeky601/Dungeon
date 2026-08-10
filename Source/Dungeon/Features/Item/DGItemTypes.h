#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "DGItemTypes.generated.h"

UENUM(BlueprintType)
enum class EDGItemType : uint8
{
	None		UMETA(Hidden),
	Consumable,
	Equipment,
	Weapon,
	Quest,
	Material
};

UENUM(BlueprintType)
enum class EDGEquipmentSlot : uint8
{
	None		UMETA(Hidden),
	Weapon,
	Helmet,
	Chest,
	Gloves,
	Boots,
	Ring,
	Necklace
};

UENUM(BlueprintType)
enum class EDGWeaponType : uint8
{
	None		UMETA(Hidden),
	Sword,
	GreatSword,
	Spear,
	Bow,
	Staff
};

// 장비 등급. 공격력/공격속도/방어력 등 기본 스탯(FDGStatBonus) 수치에는 절대 영향을 주지 않는다.
// 등급 차이는 오직 UDGEquipmentSkillTreeData 스킬트리에서 선택 가능한 노드(선택지/확장성)의 차이로만 표현한다.
UENUM(BlueprintType)
enum class EDGItemRarity : uint8
{
	None		UMETA(Hidden),
	Common,
	Rare,
	Epic,
	Legendary
};

// Stat 태그 → 보너스 수치 맵. 신규 스탯 추가 시 DGGameplayTags에 Stat.* 태그만 추가하면 되고
// 이 구조체는 수정하지 않는다 (SetByCaller_* 태그와 동일한 "태그 = 값 채널" 패턴).
USTRUCT(BlueprintType)
struct FDGStatBonus
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, meta = (Categories = "Stat"))
	TMap<FGameplayTag, float> Bonuses;
};