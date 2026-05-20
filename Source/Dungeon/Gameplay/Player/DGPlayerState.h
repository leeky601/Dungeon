#pragma once

#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "DGPlayerState.generated.h"

class UDGAbilitySystemComponent;
class UDGAttributeSet;

UCLASS()
class DUNGEON_API ADGPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ADGPlayerState();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UDGAbilitySystemComponent* GetDGAbilitySystemComponent() const { return AbilitySystemComponent; }
	const UDGAttributeSet* GetAttributeSet() const { return AttributeSet; }

private:
	UPROPERTY(VisibleAnywhere, Category="GAS")
	TObjectPtr<UDGAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UDGAttributeSet> AttributeSet;
};
