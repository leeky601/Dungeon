#pragma once

#include "NativeGameplayTags.h"

namespace DGGameplayTags
{
	// Input
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Attack_Melee)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Dodge)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Interact)

	// Character State
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Dead)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Stunned)

	// Ability
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Attack_Melee)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Dodge)

	// Damage Type
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Damage_Physical)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Damage_Magic)
}
