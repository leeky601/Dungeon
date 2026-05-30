#pragma once

#include "Abilities/GameplayAbility.h"
#include "GameplayTagContainer.h"
#include "DGGameplayAbility_LightAttack.generated.h"

// 단일 근접 공격 어빌리티.
// 흐름: 몽타주 재생 → AN_SendGameplayEvent 발동 → 서버 SphereTrace → GE_LightAttackDamage 적용
UCLASS()
class DUNGEON_API UDGGameplayAbility_LightAttack : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UDGGameplayAbility_LightAttack();

protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

private:
	// WaitGameplayEvent 콜백 — 서버 여부 확인 후 트레이스 실행
	UFUNCTION()
	void OnHitEventReceived(FGameplayEventData Payload);

	// 몽타주 완료/취소 콜백
	UFUNCTION() void OnMontageCompleted();
	UFUNCTION() void OnMontageCancelled();

	// 공격 애니메이션 몽타주 (에디터에서 할당)
	UPROPERTY(EditDefaultsOnly, Category="Attack|Animation")
	TObjectPtr<UAnimMontage> AttackMontage;

	// IncomingDamage에 적용할 Instant GE (에디터에서 GE_LightAttackDamage 할당)
	UPROPERTY(EditDefaultsOnly, Category="Attack|Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	// 전방 트레이스 거리 (cm)
	UPROPERTY(EditDefaultsOnly, Category="Attack|Trace", meta=(ClampMin="1.0", ClampMax="1000.0"))
	float AttackRange = 200.f;

	// 트레이스 구체 반지름 (cm)
	UPROPERTY(EditDefaultsOnly, Category="Attack|Trace", meta=(ClampMin="1.0", ClampMax="500.0"))
	float AttackRadius = 60.f;

	// AN_SendGameplayEvent가 전송하는 이벤트 태그와 일치해야 함
	UPROPERTY(EditDefaultsOnly, Category="Attack|Event")
	FGameplayTag HitEventTag;
};