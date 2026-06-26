#pragma once

#include "Animation/AnimInstance.h"
#include "DGAnimInstance.generated.h"

class ACharacter;
class UCharacterMovementComponent;

UCLASS()
class DUNGEON_API UDGAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	// 이동 속도 (XY 평면 기준)
	UPROPERTY(BlueprintReadOnly, Category = "Animation|Movement")
	float GroundSpeed = 0.f;

	// 입력 가속도가 존재하는지 여부
	UPROPERTY(BlueprintReadOnly, Category = "Animation|Movement")
	bool bHasAcceleration = false;

	// 공중에 있는지 여부 (Jump / Fall 판단)
	UPROPERTY(BlueprintReadOnly, Category = "Animation|Movement")
	bool bIsFalling = false;

private:
	UPROPERTY(Transient)
	TObjectPtr<APawn> OwningPawn;

	UPROPERTY(Transient)
	TObjectPtr<ACharacter> OwningCharacter;

	UPROPERTY(Transient)
	TObjectPtr<UCharacterMovementComponent> MovementComponent;
};