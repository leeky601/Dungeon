#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "DGInputData.generated.h"

class UInputAction;
class UInputMappingContext;

// InputAction + InputTag 쌍. Native / Ability 입력 모두 이 구조체를 사용한다.
USTRUCT(BlueprintType)
struct FDGInputActionConfig
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly, meta = (Categories = "Input"))
	FGameplayTag InputTag;
};

// 입력 관련 에셋을 한 곳에서 관리하는 DataAsset.
// IMC, Native 입력, Ability 입력을 모두 소유한다.
UCLASS()
class DUNGEON_API UDGInputData : public UDataAsset
{
	GENERATED_BODY()

public:
	// Enhanced Input 매핑 컨텍스트 — PlayerCharacter가 빙의 시 등록한다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mapping")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	// Native 입력 — GAS를 거치지 않고 Character에서 직접 처리 (Move / Look / Jump 등)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Native Input")
	TArray<FDGInputActionConfig> NativeInputActions;

	// GAS Ability 입력 — InputTag → ASC로 전달되어 Ability를 활성화
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Input")
	TArray<FDGInputActionConfig> AbilityInputActions;

	// NativeInputActions에서 InputTag와 일치하는 InputAction을 반환한다. 없으면 nullptr.
	const UInputAction* FindNativeActionByTag(const FGameplayTag& Tag) const;
};