#include "Gameplay/Animation/DGAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Dungeon.h"

void UDGAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwningPawn = TryGetPawnOwner();
	OwningCharacter = Cast<ACharacter>(OwningPawn);

	if (OwningCharacter)
	{
		MovementComponent = OwningCharacter->GetCharacterMovement();
	}
	else
	{
		UE_LOG(LogDungeon, Warning, TEXT("[AnimInstance] OwningPawn is not a Character: %s"),
			OwningPawn ? *OwningPawn->GetName() : TEXT("nullptr"));
	}
}

void UDGAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!MovementComponent)
	{
		return;
	}

	GroundSpeed = MovementComponent->Velocity.Size2D();
	bHasAcceleration = !MovementComponent->GetCurrentAcceleration().IsNearlyZero();
	bIsFalling = MovementComponent->IsFalling();
}