#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "Features/Item/DGItemTypes.h"
#include "Gameplay/AbilitySystem/DGAbilitySet.h"
#include "DGEquipmentSkillTreeData.generated.h"

class UGameplayEffect;

// 스킬 노드 해금 시 지급될 보상. 이번 단계에서는 순수 데이터 선언만 하며,
// 실제 ASC::GiveAbility / ApplyGameplayEffectToSelf / AddLooseGameplayTags 호출은 구현하지 않는다.
USTRUCT(BlueprintType)
struct FDGSkillNodeReward
{
	GENERATED_BODY()

	// FDGAbilitySet_GameplayAbility(AbilityClass+InputTag+Level)를 재사용해 배열에 추가하는 것만으로 확장된다.
	UPROPERTY(EditDefaultsOnly, Category = "Reward")
	TArray<FDGAbilitySet_GameplayAbility> GrantedAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "Reward")
	TArray<TSubclassOf<UGameplayEffect>> GrantedEffects;

	UPROPERTY(EditDefaultsOnly, Category = "Reward")
	FGameplayTagContainer GrantedTags;
};

// 스킬트리 노드 정적 정의. 그래프가 아닌 단순 배열 원소로 취급하며,
// 선행 조건은 RequiredPreviousNodes(AND 조건) 배열로만 표현한다.
USTRUCT(BlueprintType)
struct FDGSkillTreeNode
{
	GENERATED_BODY()

	// 노드 식별 태그. 네이티브 C++ 태그가 아니라 DataAsset/Config(GameplayTags 에디터, .ini)에서
	// 자유롭게 확장 가능한 루즈 태그로 관리한다 — 노드 추가 시 코드 수정이 필요 없다.
	UPROPERTY(EditDefaultsOnly, Category = "Node", meta = (Categories = "SkillTree.Node"))
	FGameplayTag NodeID;

	UPROPERTY(EditDefaultsOnly, Category = "Node")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, Category = "Node", meta = (MultiLine = true))
	FText Description;

	// 이 노드를 선택하기 위한 최소 세션 레벨. UDGSessionProgressComponent::GetSessionLevel()과 비교된다.
	// (Item 기능은 Session 기능을 직접 참조하지 않으므로 값은 호출자가 조회해 전달한다.)
	UPROPERTY(EditDefaultsOnly, Category = "Node|Requirements", meta = (ClampMin = "1"))
	int32 RequiredSessionLevel = 1;

	// 선행 노드 목록 — 모두 선택되어 있어야 하는 AND 조건. 비어 있으면 루트 노드.
	UPROPERTY(EditDefaultsOnly, Category = "Node|Requirements", meta = (Categories = "SkillTree.Node"))
	TArray<FGameplayTag> RequiredPreviousNodes;

	UPROPERTY(EditDefaultsOnly, Category = "Node|Reward")
	FDGSkillNodeReward Reward;
};

// 장비 스킬트리의 정적 데이터. 노드 배열은 에디터에서 작성 후 변경되지 않는다.
// 플레이어가 실제로 선택한 노드 목록(런타임 상태)은 이 애셋이 보관하지 않고,
// 장착 슬롯(주무기/보조무기 등)별로 호출자 측에서 별도 상태 컨테이너로 관리한다.
// 이를 통해 하나의 트리 애셋을 여러 슬롯이 동시에 const 참조로 공유할 수 있다.
UCLASS()
class DUNGEON_API UDGEquipmentSkillTreeData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SkillTree|Identity", meta = (Categories = "SkillTree"))
	FGameplayTag SkillTreeID;

	// 이 트리가 대응하는 무기 종류. None = 무기 종류와 무관한 범용 트리(방어구 등).
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SkillTree|Identity")
	EDGWeaponType AssociatedWeaponType = EDGWeaponType::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SkillTree|Nodes")
	TArray<FDGSkillTreeNode> Nodes;

	// 순수 조건 검사 함수 — 어떠한 상태도 변경하지 않는다(side effect 없음). 노드 선택/해금 처리는 호출자의 책임이다.
	// CurrentSessionLevel: 호출자가 UDGSessionProgressComponent::GetSessionLevel() 등에서 조회해 전달한다.
	// SelectedNodeTags: 현재까지 선택 완료된 노드 ID 목록(런타임 상태) — 이 함수는 읽기만 한다.
	UFUNCTION(BlueprintPure, Category = "SkillTree")
	bool CanSelectNode(FGameplayTag NodeID, int32 CurrentSessionLevel, const TArray<FGameplayTag>& SelectedNodeTags) const;

private:
	const FDGSkillTreeNode* FindNode(FGameplayTag NodeID) const;
};