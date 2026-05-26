#include "Features/Combat/Abilities/DGGameplayAbility_LightAttack.h"
#include "Core/Tags/DGGameplayTags.h"
#include "Dungeon.h"

UDGGameplayAbility_LightAttack::UDGGameplayAbility_LightAttack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	FGameplayTagContainer Tags;
	Tags.AddTag(DGGameplayTags::Ability_Attack_Melee_Light);
	SetAssetTags(Tags);
}

void UDGGameplayAbility_LightAttack::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UE_LOG(LogDungeon, Log, TEXT("[GAS] Light Attack Activated"));

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}