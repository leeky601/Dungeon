#include "Gameplay/Characters/DGCharacterBase.h"
#include "Gameplay/AbilitySystem/DGAbilitySystemComponent.h"
#include "Gameplay/AbilitySystem/DGAttributeSet.h"

ADGCharacterBase::ADGCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
}

UAbilitySystemComponent* ADGCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

bool ADGCharacterBase::IsAlive() const
{
	return AttributeSet && AttributeSet->GetHealth() > 0.f;
}

void ADGCharacterBase::InitializeAbilitySystem(UDGAbilitySystemComponent* InASC, AActor* InOwnerActor)
{
	if (!InASC) return;

	AbilitySystemComponent = InASC;
	AbilitySystemComponent->InitAbilityActorInfo(InOwnerActor, this);
	AttributeSet = const_cast<UDGAttributeSet*>(InASC->GetSet<UDGAttributeSet>());
}
