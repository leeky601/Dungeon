#pragma once

#include "EnhancedInputComponent.h"
#include "GameplayTagContainer.h"
#include "Gameplay/Input/DGInputData.h"
#include "DGInputComponent.generated.h"

UCLASS()
class DUNGEON_API UDGInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	// AbilityInputActions를 순회하며 Started / Completed 이벤트를 바인딩한다.
	// PressedFunc  시그니처: void(FGameplayTag InputTag)
	// ReleasedFunc 시그니처: void(FGameplayTag InputTag)
	template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
	void BindAbilityActions(
		const UDGInputData* InputData,
		UserClass* Object,
		PressedFuncType PressedFunc,
		ReleasedFuncType ReleasedFunc);

	// NativeInputActions에서 InputTag로 InputAction을 찾아 지정 TriggerEvent로 바인딩한다.
	template<class UserClass, typename FuncType>
	void BindNativeAction(
		const UDGInputData* InputData,
		const FGameplayTag& InputTag,
		ETriggerEvent TriggerEvent,
		UserClass* Object,
		FuncType Func);
};

template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
void UDGInputComponent::BindAbilityActions(
	const UDGInputData* InputData,
	UserClass* Object,
	PressedFuncType PressedFunc,
	ReleasedFuncType ReleasedFunc)
{
	if (!InputData) return;

	for (const FDGInputActionConfig& Config : InputData->AbilityInputActions)
	{
		if (!Config.InputAction || !Config.InputTag.IsValid()) continue;

		BindAction(Config.InputAction, ETriggerEvent::Started,   Object, PressedFunc,  Config.InputTag);
		BindAction(Config.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Config.InputTag);
	}
}

template<class UserClass, typename FuncType>
void UDGInputComponent::BindNativeAction(
	const UDGInputData* InputData,
	const FGameplayTag& InputTag,
	ETriggerEvent TriggerEvent,
	UserClass* Object,
	FuncType Func)
{
	if (!InputData) return;

	if (const UInputAction* IA = InputData->FindNativeActionByTag(InputTag))
	{
		BindAction(IA, TriggerEvent, Object, Func);
	}
}
