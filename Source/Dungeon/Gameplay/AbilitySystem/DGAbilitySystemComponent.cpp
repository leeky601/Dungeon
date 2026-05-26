#include "Gameplay/AbilitySystem/DGAbilitySystemComponent.h"

UDGAbilitySystemComponent::UDGAbilitySystemComponent()
{
	// Player: Mixed / Enemy: Minimal — PlayerState 소유 기준 기본값
	ReplicationMode = EGameplayEffectReplicationMode::Mixed;
}

void UDGAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	ABILITYLIST_SCOPE_LOCK();

	for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (Spec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			TryActivateAbility(Spec.Handle);
			return;
		}
	}
}

void UDGAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	ABILITYLIST_SCOPE_LOCK();

	for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (Spec.GetDynamicSpecSourceTags().HasTagExact(InputTag) && Spec.IsActive())
		{
			AbilitySpecInputReleased(Spec);
		}
	}
}
