#include "Gameplay/Characters/DGPlayerCharacter.h"
#include "Gameplay/Player/DGPlayerState.h"
#include "Gameplay/AbilitySystem/DGAbilitySystemComponent.h"
#include "Dungeon.h"

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

	UE_LOG(LogDungeon, Log,
		TEXT("[GAS] InitFromPlayerState | Actor=%s | Auth=%s | Local=%s | PS=%s"),
		*GetName(),
		HasAuthority()       ? TEXT("Server") : TEXT("Client"),
		IsLocallyControlled() ? TEXT("Yes")   : TEXT("No"),
		PS ? *PS->GetName() : TEXT("NULL"));

	if (!PS) return;

	InitializeAbilitySystem(PS->GetDGAbilitySystemComponent(), PS);

	const UDGAbilitySystemComponent* ASC = PS->GetDGAbilitySystemComponent();
	if (ASC)
	{
		UE_LOG(LogDungeon, Log,
			TEXT("[GAS] ASC Ready | Actor=%s | ASC=%s | OwnerActor=%s | AvatarActor=%s"),
			*GetName(),
			*ASC->GetName(),
			ASC->GetOwnerActor() ? *ASC->GetOwnerActor()->GetName() : TEXT("NULL"),
			ASC->GetAvatarActor() ? *ASC->GetAvatarActor()->GetName() : TEXT("NULL"));
	}
	else
	{
		UE_LOG(LogDungeon, Warning, TEXT("[GAS] ASC is NULL for %s"), *GetName());
	}
}
