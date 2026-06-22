#include "Core/Tags/DGGameplayTags.h"

namespace DGGameplayTags
{
	// ─── Input (Native) ──────────────────────────────────────────────────────
	UE_DEFINE_GAMEPLAY_TAG(Input_Native_Move, "Input.Native.Move")
	UE_DEFINE_GAMEPLAY_TAG(Input_Native_Look, "Input.Native.Look")
	UE_DEFINE_GAMEPLAY_TAG(Input_Native_Jump, "Input.Native.Jump")

	// ─── Input (Ability) ─────────────────────────────────────────────────────
	UE_DEFINE_GAMEPLAY_TAG(Input_Attack_Light,  "Input.Attack.Light")
	UE_DEFINE_GAMEPLAY_TAG(Input_Attack_Heavy,  "Input.Attack.Heavy")
	UE_DEFINE_GAMEPLAY_TAG(Input_Dodge,         "Input.Dodge")
	UE_DEFINE_GAMEPLAY_TAG(Input_Interact,      "Input.Interact")
	UE_DEFINE_GAMEPLAY_TAG(Input_Skill_1,       "Input.Skill.1")
	UE_DEFINE_GAMEPLAY_TAG(Input_Skill_2,       "Input.Skill.2")
	UE_DEFINE_GAMEPLAY_TAG(Input_Skill_3,       "Input.Skill.3")

	// ─── Ability ─────────────────────────────────────────────────────────────
	UE_DEFINE_GAMEPLAY_TAG(Ability_Attack_Melee_Light, "Ability.Attack.Melee.Light")
	UE_DEFINE_GAMEPLAY_TAG(Ability_Attack_Melee_Heavy, "Ability.Attack.Melee.Heavy")
	UE_DEFINE_GAMEPLAY_TAG(Ability_Dodge,              "Ability.Dodge")
	UE_DEFINE_GAMEPLAY_TAG(Ability_Interact,           "Ability.Interact")
	UE_DEFINE_GAMEPLAY_TAG(Ability_Skill_1,            "Ability.Skill.1")
	UE_DEFINE_GAMEPLAY_TAG(Ability_Skill_2,            "Ability.Skill.2")
	UE_DEFINE_GAMEPLAY_TAG(Ability_Skill_3,            "Ability.Skill.3")

	// ─── State ───────────────────────────────────────────────────────────────
	UE_DEFINE_GAMEPLAY_TAG(State_Dead,       "State.Dead")
	UE_DEFINE_GAMEPLAY_TAG(State_Stunned,    "State.Stunned")
	UE_DEFINE_GAMEPLAY_TAG(State_Knockback,  "State.Knockback")
	UE_DEFINE_GAMEPLAY_TAG(State_Invincible, "State.Invincible")
	UE_DEFINE_GAMEPLAY_TAG(State_Casting,    "State.Casting")

	// ─── Event ───────────────────────────────────────────────────────────────
	UE_DEFINE_GAMEPLAY_TAG(Event_Combat_Hit,              "Event.Combat.Hit")
	UE_DEFINE_GAMEPLAY_TAG(Event_Combat_Died,             "Event.Combat.Died")
	UE_DEFINE_GAMEPLAY_TAG(Event_Combat_ComboReady,       "Event.Combat.ComboReady")
	UE_DEFINE_GAMEPLAY_TAG(Event_Interaction_Triggered,   "Event.Interaction.Triggered")

	// ─── Damage ──────────────────────────────────────────────────────────────
	UE_DEFINE_GAMEPLAY_TAG(Damage_Physical, "Damage.Physical")
	UE_DEFINE_GAMEPLAY_TAG(Damage_Magic,    "Damage.Magic")
	UE_DEFINE_GAMEPLAY_TAG(Damage_True,     "Damage.True")

	// ─── SetByCaller ─────────────────────────────────────────────────────────
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_Damage_Base,  "SetByCaller.Damage.Base")
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_Damage_Bonus, "SetByCaller.Damage.Bonus")
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_HealAmount,   "SetByCaller.HealAmount")
}