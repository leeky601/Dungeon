#include "Features/Combat/Components/DGCombatComponent.h"

UDGCombatComponent::UDGCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UDGCombatComponent::ResetCombo()
{
	ComboCount = 0;
}
