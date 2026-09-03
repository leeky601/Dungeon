#pragma once

#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "DGPlayerState.generated.h"

class UDGAbilitySystemComponent;
class UDGAttributeSet;
class UDGSessionProgressComponent;
class UDGSessionInventoryComponent;

UCLASS()
class DUNGEON_API ADGPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ADGPlayerState();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UDGAbilitySystemComponent* GetDGAbilitySystemComponent() const { return AbilitySystemComponent; }
	const UDGAttributeSet* GetAttributeSet() const { return AttributeSet; }
	UDGSessionProgressComponent* GetSessionProgressComponent() const { return SessionProgressComponent; }
	UDGSessionInventoryComponent* GetSessionInventoryComponent() const { return SessionInventoryComponent; }

private:
	UPROPERTY(VisibleAnywhere, Category="GAS")
	TObjectPtr<UDGAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UDGAttributeSet> AttributeSet;

	UPROPERTY(VisibleAnywhere, Category="Session")
	TObjectPtr<UDGSessionProgressComponent> SessionProgressComponent;

	UPROPERTY(VisibleAnywhere, Category="Session")
	TObjectPtr<UDGSessionInventoryComponent> SessionInventoryComponent;
};
