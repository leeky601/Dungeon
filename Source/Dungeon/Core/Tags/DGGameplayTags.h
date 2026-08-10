#pragma once

#include "NativeGameplayTags.h"

namespace DGGameplayTags
{
	// ─── Input (Native) ──────────────────────────────────────────────────────
	// GAS를 거치지 않고 Character에서 직접 처리하는 Native 입력 태그.
	// DGInputConfig::NativeInputActions에서 InputAction을 찾는 키로 사용한다.
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Native_Move)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Native_Look)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Native_Jump)

	// ─── Input (Ability) ─────────────────────────────────────────────────────
	// IA(Input Action)와 1:1 대응. ASC TryActivateAbilitiesByTag()의 인자로 사용.
	// AbilitySet DataAsset에서 InputTag → AbilityClass 매핑을 보관한다.
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Attack_Light)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Attack_Heavy)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Dodge)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Interact)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Skill_1)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Skill_2)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Skill_3)

	// ─── Ability ─────────────────────────────────────────────────────────────
	// GameplayAbility의 AbilityTags / BlockAbilitiesWithTag / CancelAbilitiesWithTag에 사용.
	// 활성 중일 때 ActivationOwnedTags로 캐릭터에 자동 부여된다.
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Attack_Melee_Light)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Attack_Melee_Heavy)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Dodge)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Interact)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Skill_1)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Skill_2)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Skill_3)

	// ─── State ───────────────────────────────────────────────────────────────
	// GE(GameplayEffect) Duration으로 부여·해제된다.
	// HasMatchingGameplayTag()로 체크. Ability의 ActivationRequiredTags / BlockedTags에 사용.
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Dead)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Stunned)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Knockback)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Invincible)   // 무적 프레임 (Dodge 등)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Casting)      // 시전 중 이동 제한 등

	// ─── Event ───────────────────────────────────────────────────────────────
	// SendGameplayEvent / WaitGameplayEvent 통신 채널.
	// 주용도: AnimNotify → Ability, Ability 간 콤보 체이닝.
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Combat_Hit)         // AnimNotify → 히트 체크 타이밍
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Combat_Died)        // 사망 → 사망 처리 Ability 트리거
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Combat_ComboReady)  // 콤보 입력 허용 윈도우 시작
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Interaction_Triggered)

	// ─── Damage ──────────────────────────────────────────────────────────────
	// 피해 유형 분류 태그. SetByCaller 키가 아님.
	// 용도: DamageExecution 분기, GameplayCue 선택, 방어/저항 계산.
	// GE의 GrantedTags 또는 Execution 로직에서 참조한다.
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Damage_Physical)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Damage_Magic)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Damage_True)   // 방어 무시

	// ─── SetByCaller ─────────────────────────────────────────────────────────
	// GE Spec에 런타임 수치를 주입하는 값 채널 태그. 피해 유형과 무관.
	// Damage.* 태그와 혼용 금지 — 타입은 Damage.*, 수치는 SetByCaller.*로 분리한다.
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Damage_Base)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Damage_Bonus)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_HealAmount)

	// ─── Stat ────────────────────────────────────────────────────────────────
	// 장비 스탯 보너스 값 채널 태그. FDGStatBonus::Bonuses의 키로 사용.
	// 신규 스탯 추가 시 여기에 태그만 추가하면 되고 FDGStatBonus는 수정하지 않는다.
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Stat_Attack)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Stat_Defense)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Stat_Strength)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Stat_Dexterity)
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Stat_CriticalRate)

	// ─── GameplayCue ─────────────────────────────────────────────────────────
	// GAS 규칙상 "GameplayCue."로 시작해야 한다.
	// Cosmetic 전용(사운드, 파티클). Dedicated Server에서 실행되지 않는다.
	// NativeGameplayTag 선언 없이 DataTable(DT_GameplayCues)로 관리한다.
	// 여기서 선언하지 않음 — 에디터 DataTable에 추가하면 GC Manager가 자동 등록한다.
}