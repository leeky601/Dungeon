#pragma once

#include "Engine/DataAsset.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayTagContainer.h"
#include "DGAbilitySet.generated.h"

class UDGAbilitySystemComponent;

USTRUCT(BlueprintType)
struct FDGAbilitySet_GameplayAbility
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayAbility> AbilityClass;

	// ASC DynamicAbilityTags에 등록 — 입력 활성화 키로 사용된다.
	UPROPERTY(EditDefaultsOnly, meta = (Categories = "Input"))
	FGameplayTag InputTag;

	UPROPERTY(EditDefaultsOnly)
	int32 AbilityLevel = 1;
};

// Ability 지급 정보를 담는 DataAsset.
// UPrimaryDataAsset 기반: 향후 AssetManager를 통한 타입 등록 / 비동기 로드에 대응한다.
UCLASS()
class DUNGEON_API UDGAbilitySet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// 서버 권한에서만 ASC에 Ability를 부여한다.
	// Character의 HasAuthority 체크 이후 호출하더라도 내부에서 권한을 재확인한다.
	void GiveToAbilitySystem(UDGAbilitySystemComponent* ASC) const;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FDGAbilitySet_GameplayAbility> GrantedAbilities;
};
