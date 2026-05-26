#pragma once

#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "DGAbilitySystemComponent.generated.h"

UCLASS()
class DUNGEON_API UDGAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UDGAbilitySystemComponent();

	// DynamicAbilityTags에 InputTag가 등록된 어빌리티를 활성화한다.
	void AbilityInputTagPressed(const FGameplayTag& InputTag);

	// 활성 중인 어빌리티에 입력 릴리즈를 통지한다.
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
};
