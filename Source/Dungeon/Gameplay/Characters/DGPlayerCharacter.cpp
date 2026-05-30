#include "Gameplay/Characters/DGPlayerCharacter.h"
#include "Gameplay/Player/DGPlayerState.h"
#include "Gameplay/AbilitySystem/DGAbilitySystemComponent.h"
#include "Gameplay/AbilitySystem/DGAbilitySet.h"
#include "Gameplay/Input/DGInputComponent.h"
#include "EnhancedInputSubsystems.h"
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

void ADGPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (APlayerController* PC = GetController<APlayerController>())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}

	UDGInputComponent* DGInput = Cast<UDGInputComponent>(PlayerInputComponent);
	if (!DGInput)
	{
		UE_LOG(LogDungeon, Warning,
			TEXT("[Input] PlayerInputComponent가 UDGInputComponent가 아닙니다. "
				 "BP_DGPlayerController의 InputComponentClass를 UDGInputComponent로 설정해주세요."));
		return;
	}

	if (!InputConfig)
	{
		UE_LOG(LogDungeon, Warning, TEXT("[Input] InputConfig가 설정되지 않았습니다. Actor=%s"), *GetName());
		return;
	}

	DGInput->BindAbilityActions(InputConfig, this,
		&ADGPlayerCharacter::OnAbilityInputTagPressed,
		&ADGPlayerCharacter::OnAbilityInputTagReleased);
}

void ADGPlayerCharacter::OnAbilityInputTagPressed(const FGameplayTag InputTag)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->AbilityInputTagPressed(InputTag);
	}
}

void ADGPlayerCharacter::OnAbilityInputTagReleased(const FGameplayTag InputTag)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->AbilityInputTagReleased(InputTag);
	}
}

void ADGPlayerCharacter::InitFromPlayerState()
{
	ADGPlayerState* PS = GetPlayerState<ADGPlayerState>();

	UE_LOG(LogDungeon, Log,
		TEXT("[GAS] InitFromPlayerState | Actor=%s | Auth=%s | Local=%s | PS=%s"),
		*GetName(),
		HasAuthority()        ? TEXT("Server") : TEXT("Client"),
		IsLocallyControlled() ? TEXT("Yes")    : TEXT("No"),
		PS ? *PS->GetName() : TEXT("NULL"));

	if (!PS) return;

	InitializeAbilitySystem(PS->GetDGAbilitySystemComponent(), PS);

	if (HasAuthority() && !bDefaultAbilitiesGranted)
	{
		if (DefaultAbilitySet)
		{
			DefaultAbilitySet->GiveToAbilitySystem(AbilitySystemComponent);
			bDefaultAbilitiesGranted = true;
		}
	}

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