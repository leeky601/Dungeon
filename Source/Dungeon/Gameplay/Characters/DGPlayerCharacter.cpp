#include "Gameplay/Characters/DGPlayerCharacter.h"
#include "Gameplay/Player/DGPlayerState.h"
#include "Gameplay/AbilitySystem/DGAbilitySystemComponent.h"
#include "Gameplay/AbilitySystem/DGAbilitySet.h"
#include "Gameplay/Input/DGInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Core/Tags/DGGameplayTags.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Dungeon.h"

ADGPlayerCharacter::ADGPlayerCharacter()
{
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(GetRootComponent());
	SpringArmComponent->TargetArmLength = 400.f;
	SpringArmComponent->bUsePawnControlRotation = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
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

	UDGInputComponent* DGInput = Cast<UDGInputComponent>(PlayerInputComponent);
	if (!DGInput)
	{
		UE_LOG(LogDungeon, Warning,
			TEXT("[Input] PlayerInputComponent가 UDGInputComponent가 아닙니다. "
				 "BP_DGPlayerController의 InputComponentClass를 UDGInputComponent로 설정해주세요."));
		return;
	}

	if (!InputData)
	{
		UE_LOG(LogDungeon, Warning, TEXT("[Input] InputData가 설정되지 않았습니다. Actor=%s"), *GetName());
		return;
	}

	if (APlayerController* PC = GetController<APlayerController>())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			if (InputData->DefaultMappingContext)
			{
				Subsystem->AddMappingContext(InputData->DefaultMappingContext, 0);
			}
		}
	}

	DGInput->BindAbilityActions(InputData, this,
		&ADGPlayerCharacter::OnAbilityInputTagPressed,
		&ADGPlayerCharacter::OnAbilityInputTagReleased);

	DGInput->BindNativeAction(InputData, DGGameplayTags::Input_Native_Move, ETriggerEvent::Triggered, this, &ADGPlayerCharacter::Input_Move);
	DGInput->BindNativeAction(InputData, DGGameplayTags::Input_Native_Look, ETriggerEvent::Triggered, this, &ADGPlayerCharacter::Input_Look);
	DGInput->BindNativeAction(InputData, DGGameplayTags::Input_Native_Jump, ETriggerEvent::Started,   this, &ADGPlayerCharacter::Input_Jump);
	DGInput->BindNativeAction(InputData, DGGameplayTags::Input_Native_Jump, ETriggerEvent::Completed, this, &ADGPlayerCharacter::Input_StopJumping);
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

void ADGPlayerCharacter::Input_Move(const FInputActionValue& Value)
{
	const FVector2D MoveInput = Value.Get<FVector2D>();
	if (!Controller) return;

	const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
	const FVector ForwardDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDir   = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDir, MoveInput.Y);
	AddMovementInput(RightDir,   MoveInput.X);
}

void ADGPlayerCharacter::Input_Look(const FInputActionValue& Value)
{
	const FVector2D LookInput = Value.Get<FVector2D>();
	AddControllerYawInput(LookInput.X);
	AddControllerPitchInput(LookInput.Y);
}

void ADGPlayerCharacter::Input_Jump()
{
	Jump();
}

void ADGPlayerCharacter::Input_StopJumping()
{
	StopJumping();
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