#include "Gameplay/Input/DGInputData.h"

const UInputAction* UDGInputData::FindNativeActionByTag(const FGameplayTag& Tag) const
{
	for (const FDGInputActionConfig& Config : NativeInputActions)
	{
		if (Config.InputTag == Tag)
		{
			return Config.InputAction;
		}
	}
	return nullptr;
}