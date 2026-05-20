#include "Core/Tags/DGGameplayTags.h"

namespace DGGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG(Input_Attack_Melee, "Input.Attack.Melee")
	UE_DEFINE_GAMEPLAY_TAG(Input_Dodge,        "Input.Dodge")
	UE_DEFINE_GAMEPLAY_TAG(Input_Interact,     "Input.Interact")

	UE_DEFINE_GAMEPLAY_TAG(State_Dead,    "State.Dead")
	UE_DEFINE_GAMEPLAY_TAG(State_Stunned, "State.Stunned")

	UE_DEFINE_GAMEPLAY_TAG(Ability_Attack_Melee, "Ability.Attack.Melee")
	UE_DEFINE_GAMEPLAY_TAG(Ability_Dodge,        "Ability.Dodge")

	UE_DEFINE_GAMEPLAY_TAG(Damage_Physical, "Damage.Physical")
	UE_DEFINE_GAMEPLAY_TAG(Damage_Magic,    "Damage.Magic")
}
