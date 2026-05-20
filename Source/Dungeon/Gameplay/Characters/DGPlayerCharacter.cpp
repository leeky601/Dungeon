#include "Gameplay/Characters/DGPlayerCharacter.h"
#include "Gameplay/Player/DGPlayerState.h"

ADGPlayerCharacter::ADGPlayerCharacter()
{
}

void ADGPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	InitFromPlayerState();
}

void ADGPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	InitFromPlayerState();
}

void ADGPlayerCharacter::InitFromPlayerState()
{
	ADGPlayerState* PS = GetPlayerState<ADGPlayerState>();
	if (!PS) return;

	InitializeAbilitySystem(PS->GetDGAbilitySystemComponent(), PS);
}
