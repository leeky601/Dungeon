#pragma once

#include "EnhancedInputComponent.h"
#include "Gameplay/Input/DGInputConfig.h"
#include "DGInputComponent.generated.h"

UCLASS()
class DUNGEON_API UDGInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	// InputConfig의 모든 AbilityBinding에 대해 Started / Completed 이벤트를 각각 바인딩한다.
	// PressedFunc  시그니처: void(FGameplayTag InputTag)
	// ReleasedFunc 시그니처: void(FGameplayTag InputTag)
	template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
	void BindAbilityActions(
		const UDGInputConfig* InputConfig,
		UserClass* Object,
		PressedFuncType PressedFunc,
		ReleasedFuncType ReleasedFunc);

	// 이동/시점 등 개별 Native InputAction을 바인딩하는 헬퍼 (비 GAS 입력용)
	template<class UserClass, typename FuncType>
	void BindNativeAction(
		const UInputAction* InputAction,
		ETriggerEvent TriggerEvent,
		UserClass* Object,
		FuncType Func);
};

template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
void UDGInputComponent::BindAbilityActions(
	const UDGInputConfig* InputConfig,
	UserClass* Object,
	PressedFuncType PressedFunc,
	ReleasedFuncType ReleasedFunc)
{
	if (!InputConfig) return;

	for (const FDGInputAbilityBinding& Binding : InputConfig->AbilityBindings)
	{
		if (!Binding.InputAction || !Binding.InputTag.IsValid()) continue;

		BindAction(Binding.InputAction, ETriggerEvent::Started,   Object, PressedFunc,  Binding.InputTag);
		BindAction(Binding.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Binding.InputTag);
	}
}

template<class UserClass, typename FuncType>
void UDGInputComponent::BindNativeAction(
	const UInputAction* InputAction,
	ETriggerEvent TriggerEvent,
	UserClass* Object,
	FuncType Func)
{
	if (!InputAction) return;
	BindAction(InputAction, TriggerEvent, Object, Func);
}