#include "Gameplay/AbilitySystem/DGAbilitySystemComponent.h"

UDGAbilitySystemComponent::UDGAbilitySystemComponent()
{
	// Player: Mixed / Enemy: Minimal — PlayerState 소유 기준 기본값
	ReplicationMode = EGameplayEffectReplicationMode::Mixed;
}
