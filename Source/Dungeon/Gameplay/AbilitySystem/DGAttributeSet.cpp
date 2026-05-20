#include "Gameplay/AbilitySystem/DGAttributeSet.h"
#include "Core/Tags/DGGameplayTags.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"

UDGAttributeSet::UDGAttributeSet()
{
	InitHealth(100.f);
	InitMaxHealth(100.f);
	InitMana(50.f);
	InitMaxMana(50.f);
	InitStamina(100.f);
	InitMaxStamina(100.f);
	InitAttackPower(10.f);
	InitDefense(5.f);
	InitMoveSpeed(600.f);
	InitIncomingDamage(0.f);
}

void UDGAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UDGAttributeSet, Health,      COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDGAttributeSet, MaxHealth,   COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDGAttributeSet, Mana,        COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDGAttributeSet, MaxMana,     COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDGAttributeSet, Stamina,     COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDGAttributeSet, MaxStamina,  COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDGAttributeSet, AttackPower, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDGAttributeSet, Defense,     COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDGAttributeSet, MoveSpeed,   COND_None, REPNOTIFY_Always);
}

void UDGAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if      (Attribute == GetHealthAttribute())  NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	else if (Attribute == GetManaAttribute())    NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
	else if (Attribute == GetStaminaAttribute()) NewValue = FMath::Clamp(NewValue, 0.f, GetMaxStamina());
}

void UDGAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute != GetIncomingDamageAttribute()) return;

	const float Damage = GetIncomingDamage();
	SetIncomingDamage(0.f);

	if (Damage <= 0.f) return;

	SetHealth(FMath::Clamp(GetHealth() - Damage, 0.f, GetMaxHealth()));

	if (GetHealth() <= 0.f)
	{
		if (UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent())
		{
			ASC->AddLooseGameplayTag(DGGameplayTags::State_Dead);
		}
	}
}

void UDGAttributeSet::OnRep_Health(const FGameplayAttributeData& Old)      { GAMEPLAYATTRIBUTE_REPNOTIFY(UDGAttributeSet, Health, Old); }
void UDGAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& Old)   { GAMEPLAYATTRIBUTE_REPNOTIFY(UDGAttributeSet, MaxHealth, Old); }
void UDGAttributeSet::OnRep_Mana(const FGameplayAttributeData& Old)        { GAMEPLAYATTRIBUTE_REPNOTIFY(UDGAttributeSet, Mana, Old); }
void UDGAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& Old)     { GAMEPLAYATTRIBUTE_REPNOTIFY(UDGAttributeSet, MaxMana, Old); }
void UDGAttributeSet::OnRep_Stamina(const FGameplayAttributeData& Old)     { GAMEPLAYATTRIBUTE_REPNOTIFY(UDGAttributeSet, Stamina, Old); }
void UDGAttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& Old)  { GAMEPLAYATTRIBUTE_REPNOTIFY(UDGAttributeSet, MaxStamina, Old); }
void UDGAttributeSet::OnRep_AttackPower(const FGameplayAttributeData& Old) { GAMEPLAYATTRIBUTE_REPNOTIFY(UDGAttributeSet, AttackPower, Old); }
void UDGAttributeSet::OnRep_Defense(const FGameplayAttributeData& Old)     { GAMEPLAYATTRIBUTE_REPNOTIFY(UDGAttributeSet, Defense, Old); }
void UDGAttributeSet::OnRep_MoveSpeed(const FGameplayAttributeData& Old)   { GAMEPLAYATTRIBUTE_REPNOTIFY(UDGAttributeSet, MoveSpeed, Old); }
