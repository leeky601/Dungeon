#pragma once

#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "DGCharacterBase.generated.h"

class UDGAbilitySystemComponent;
class UDGAttributeSet;

UCLASS(Abstract)
class DUNGEON_API ADGCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ADGCharacterBase();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	bool IsAlive() const;

protected:
	// 서브클래스에서 ASC 준비된 시점에 호출
	// Player: PossessedBy / OnRep_PlayerState
	// Enemy:  BeginPlay (자체 ASC 소유)
	void InitializeAbilitySystem(UDGAbilitySystemComponent* InASC, AActor* InOwnerActor);

	UPROPERTY()
	TObjectPtr<UDGAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UDGAttributeSet> AttributeSet;
};
