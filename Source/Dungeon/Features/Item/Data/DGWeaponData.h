#pragma once

#include "Features/Item/Data/DGEquipmentData.h"
#include "Features/Item/DGItemTypes.h"
#include "Gameplay/AbilitySystem/DGAbilitySet.h"
#include "DGWeaponData.generated.h"

class AActor;
class UStaticMesh;
class UAnimMontage;
class UAnimInstance;
class UDGEquipmentSkillTreeData;

// 무기 아이템 데이터. EquipmentData를 상속해 무기 전용 메시 / 애니메이션 / 어빌리티를 확장한다.
UCLASS()
class DUNGEON_API UDGWeaponData : public UDGEquipmentData
{
	GENERATED_BODY()

public:
	// ─── Weapon ──────────────────────────────────────────────────────────
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	EDGWeaponType WeaponType = EDGWeaponType::None;

	// 무기 분류 태그 — Ability 활성화 조건, GameplayCue 분기 등에 사용
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FGameplayTagContainer GameplayTags;

	// ─── Visual ──────────────────────────────────────────────────────────
	// 장착 시 손 소켓에 부착되는 무기 메시
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Visual")
	TSoftObjectPtr<UStaticMesh> WeaponMesh;

	// 콜리전/이펙트 등 부가 로직이 필요한 무기는 WeaponMesh 대신 이 액터를 스폰해 부착한다 (선택 사항).
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Visual")
	TSubclassOf<AActor> WeaponActorClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Visual")
	FName EquipSocketName;

	// ─── Animation ───────────────────────────────────────────────────────
	// 무기 종류별 전투 포즈를 주입하는 Linked Anim Layer
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Animation")
	TSubclassOf<UAnimInstance> LinkedAnimLayerClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Animation")
	TSoftObjectPtr<UAnimMontage> EquipMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Animation")
	TSoftObjectPtr<UAnimMontage> UnequipMontage;

	// ─── Abilities ───────────────────────────────────────────────────────
	// 콤보 시스템의 고정 슬롯 — Input_Attack_Light/Heavy에 대응한다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Abilities")
	TSubclassOf<UGameplayAbility> DefaultAttackAbility;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Abilities")
	TSubclassOf<UGameplayAbility> DefaultHeavyAttackAbility;

	// 무기별 추가 스킬. FDGAbilitySet_GameplayAbility(AbilityClass+InputTag+Level)를 재사용해
	// 새 스킬을 배열에 추가하는 것만으로 확장되며 코드 수정이 필요 없다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Abilities")
	TArray<FDGAbilitySet_GameplayAbility> AdditionalWeaponAbilities;

	// ─── SkillTree ───────────────────────────────────────────────────────
	// 이 무기가 사용하는 스킬트리 정적 데이터. Rarity는 StatBonus 수치가 아니라
	// "어떤 SkillTreeData를 참조하는가"로만 차이를 표현한다 — 등급별로 WeaponData 애셋 자체가
	// 분리되므로(예: WeaponData_Sword_Common / WeaponData_Sword_Legendary), 이 필드는 등급별 배열/맵이 아닌
	// 단일 참조로 충분하다. 실제 등급 차이는 에디터에서 상위 등급 애셋에 노드가 더 많은
	// SkillTreeData를 연결하는 방식으로 표현한다.
	// 다른 PrimaryDataAsset("SkillTree" 타입)이므로 WeaponData 로드 시 함께 강제 로드되지 않도록 Soft 참조를 사용한다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|SkillTree")
	TSoftObjectPtr<UDGEquipmentSkillTreeData> SkillTreeData;

	// WeaponType과 InSkillTreeData::AssociatedWeaponType 일치 여부만 검사하는 순수 함수 — 로딩은 호출자 책임.
	// AssociatedWeaponType이 None이면 무기 종류에 무관한 범용 트리로 간주해 항상 호환되는 것으로 판단한다.
	UFUNCTION(BlueprintPure, Category = "Weapon|SkillTree")
	bool IsSkillTreeCompatible(const UDGEquipmentSkillTreeData* InSkillTreeData) const;

#if WITH_EDITOR
	// 에디터 애셋 검증: SkillTreeData가 지정돼 있다면 동기 로드 후 WeaponType과의 호환 여부를 확인한다.
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
};