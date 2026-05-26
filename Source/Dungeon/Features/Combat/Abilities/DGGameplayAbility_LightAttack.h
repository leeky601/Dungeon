#pragma once

#include "Abilities/GameplayAbility.h"
#include "DGGameplayAbility_LightAttack.generated.h"

UCLASS()
class DUNGEON_API UDGGameplayAbility_LightAttack : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UDGGameplayAbility_LightAttack();

protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
};