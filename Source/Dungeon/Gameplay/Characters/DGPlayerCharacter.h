#pragma once

#include "Gameplay/Characters/DGCharacterBase.h"
#include "GameplayTagContainer.h"
#include "InputActionValue.h"
#include "DGPlayerCharacter.generated.h"

class UDGAbilitySet;
class UDGInputData;
class USpringArmComponent;
class UCameraComponent;

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(EditDefaultsOnly, Category = "GAS")
	TObjectPtr<UDGAbilitySet> DefaultAbilitySet;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UDGInputData> InputData;

private:
	void InitFromPlayerState();
	 
	void OnAbilityInputTagPressed(const FGameplayTag InputTag);
	void OnAbilityInputTagReleased(const FGameplayTag InputTag);

	void Input_Move(const FInputActionValue& Value);
	void Input_Look(const FInputActionValue& Value);
	void Input_Jump();
	void Input_StopJumping();

	bool bDefaultAbilitiesGranted = false;
};