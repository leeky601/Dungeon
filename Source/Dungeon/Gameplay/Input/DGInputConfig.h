#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "DGInputConfig.generated.h"

class UInputAction;

USTRUCT(BlueprintType)
struct FDGInputAbilityBinding
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly,meta = (Categories = "Input"))
	FGameplayTag InputTag;
};

// InputAction → InputTag 매핑을 보관하는 DataAsset.
// BP_PlayerCharacter의 InputConfig 슬롯에 할당한다.
UCLASS()
class DUNGEON_API UDGInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FDGInputAbilityBinding> AbilityBindings;
};