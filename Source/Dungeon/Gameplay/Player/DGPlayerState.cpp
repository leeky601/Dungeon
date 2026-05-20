#include "Gameplay/Player/DGPlayerState.h"
#include "Gameplay/AbilitySystem/DGAbilitySystemComponent.h"
#include "Gameplay/AbilitySystem/DGAttributeSet.h"

ADGPlayerState::ADGPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UDGAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	AttributeSet = CreateDefaultSubobject<UDGAttributeSet>(TEXT("AttributeSet"));

	// GAS Attribute 변경이 빠르게 복제되도록 업데이트 빈도 높임
	SetNetUpdateFrequency(100.f);
}

UAbilitySystemComponent* ADGPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
