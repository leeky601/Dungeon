#pragma once

#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "DGAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName)			\
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName)	\
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName)				\
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName)				\
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class DUNGEON_API UDGAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UDGAttributeSet();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	// Vital
	UPROPERTY(BlueprintReadOnly, Category="Attributes|Vital", ReplicatedUsing=OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UDGAttributeSet, Health)

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Vital", ReplicatedUsing=OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UDGAttributeSet, MaxHealth)

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Vital", ReplicatedUsing=OnRep_Mana)
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UDGAttributeSet, Mana)

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Vital", ReplicatedUsing=OnRep_MaxMana)
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UDGAttributeSet, MaxMana)

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Vital", ReplicatedUsing=OnRep_Stamina)
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(UDGAttributeSet, Stamina)

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Vital", ReplicatedUsing=OnRep_MaxStamina)
	FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS(UDGAttributeSet, MaxStamina)

	// Combat
	UPROPERTY(BlueprintReadOnly, Category="Attributes|Combat", ReplicatedUsing=OnRep_AttackPower)
	FGameplayAttributeData AttackPower;
	ATTRIBUTE_ACCESSORS(UDGAttributeSet, AttackPower)

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Combat", ReplicatedUsing=OnRep_Defense)
	FGameplayAttributeData Defense;
	ATTRIBUTE_ACCESSORS(UDGAttributeSet, Defense)

	// Movement
	UPROPERTY(BlueprintReadOnly, Category="Attributes|Movement", ReplicatedUsing=OnRep_MoveSpeed)
	FGameplayAttributeData MoveSpeed;
	ATTRIBUTE_ACCESSORS(UDGAttributeSet, MoveSpeed)

	// Meta — 복제 없음, PostGameplayEffectExecute에서 소비 후 0으로 초기화
	UPROPERTY(BlueprintReadOnly, Category="Attributes|Meta")
	FGameplayAttributeData IncomingDamage;
	ATTRIBUTE_ACCESSORS(UDGAttributeSet, IncomingDamage)

protected:
	UFUNCTION() void OnRep_Health(const FGameplayAttributeData& Old);
	UFUNCTION() void OnRep_MaxHealth(const FGameplayAttributeData& Old);
	UFUNCTION() void OnRep_Mana(const FGameplayAttributeData& Old);
	UFUNCTION() void OnRep_MaxMana(const FGameplayAttributeData& Old);
	UFUNCTION() void OnRep_Stamina(const FGameplayAttributeData& Old);
	UFUNCTION() void OnRep_MaxStamina(const FGameplayAttributeData& Old);
	UFUNCTION() void OnRep_AttackPower(const FGameplayAttributeData& Old);
	UFUNCTION() void OnRep_Defense(const FGameplayAttributeData& Old);
	UFUNCTION() void OnRep_MoveSpeed(const FGameplayAttributeData& Old);
};
