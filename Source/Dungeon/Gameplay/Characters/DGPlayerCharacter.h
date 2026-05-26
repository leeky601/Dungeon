#pragma once

#include "Gameplay/Characters/DGCharacterBase.h"
#include "GameplayTagContainer.h"
#include "DGPlayerCharacter.generated.h"

class UDGAbilitySet;
class UDGInputConfig;
class UInputMappingContext;

UCLASS()
class DUNGEON_API ADGPlayerCharacter : public ADGCharacterBase
{
	GENERATED_BODY()

public:
	ADGPlayerCharacter();

protected:
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditDefaultsOnly, Category = "GAS")
	TObjectPtr<UDGAbilitySet> DefaultAbilitySet;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UDGInputConfig> InputConfig;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

private:
	void InitFromPlayerState();
	void OnAbilityInputTagPressed(FGameplayTag InputTag);
	void OnAbilityInputTagReleased(FGameplayTag InputTag);

	bool bDefaultAbilitiesGranted = false;
};