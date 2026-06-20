#include "Features/Combat/Abilities/DGGameplayAbility_LightAttack.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Gameplay/AbilitySystem/DGAttributeSet.h"
#include "Core/Tags/DGGameplayTags.h"
#include "Dungeon.h"

UDGGameplayAbility_LightAttack::UDGGameplayAbility_LightAttack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	FGameplayTagContainer Tags;
	Tags.AddTag(DGGameplayTags::Ability_Attack_Melee_Light);
	SetAssetTags(Tags);

	HitEventTag  = DGGameplayTags::Event_Combat_Hit;
	AttackRange  = 200.f;
	AttackRadius = 60.f;
}

void UDGGameplayAbility_LightAttack::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!AttackMontage)
	{
		UE_LOG(LogDungeon, Warning, TEXT("[GA_LightAttack] AttackMontage가 할당되지 않았습니다 — 어빌리티 중단"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 몽타주 재생 태스크
	UAbilityTask_PlayMontageAndWait* MontageTask =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, AttackMontage);
	MontageTask->OnCompleted.AddDynamic(this, &UDGGameplayAbility_LightAttack::OnMontageCompleted);
	MontageTask->OnBlendOut.AddDynamic(this, &UDGGameplayAbility_LightAttack::OnMontageCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &UDGGameplayAbility_LightAttack::OnMontageCancelled);
	MontageTask->OnCancelled.AddDynamic(this, &UDGGameplayAbility_LightAttack::OnMontageCancelled);
	MontageTask->ReadyForActivation();

	// AnimNotify_GameplayEvent로부터 히트 타이밍 이벤트 수신 대기
	UAbilityTask_WaitGameplayEvent* EventTask =
		UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, HitEventTag, nullptr, false, true);
	EventTask->EventReceived.AddDynamic(this, &UDGGameplayAbility_LightAttack::OnHitEventReceived);
	EventTask->ReadyForActivation();
}

void UDGGameplayAbility_LightAttack::OnHitEventReceived(FGameplayEventData Payload)
{
	const AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor) return;

	const bool bHasAuthority = AvatarActor->HasAuthority();
	const bool bIsLocallyControlled = [AvatarActor]() -> bool
	{
		if (const APawn* Pawn = Cast<APawn>(AvatarActor)) return Pawn->IsLocallyControlled();
		return false;
	}();

	UE_LOG(LogDungeon, Log,
		TEXT("[GA_LightAttack] OnHitEventReceived | Tag=%s | Authority=%s | LocalControlled=%s"),
		*HitEventTag.ToString(),
		bHasAuthority        ? TEXT("YES") : TEXT("NO"),
		bIsLocallyControlled ? TEXT("YES") : TEXT("NO"));

	// 실제 판정은 서버 전용
	if (!bHasAuthority) return;

	if (!DamageEffectClass)
	{
		UE_LOG(LogDungeon, Warning, TEXT("[GA_LightAttack] DamageEffectClass가 할당되지 않았습니다 — 트레이스 생략"));
		return;
	}

	// 전방 SphereTrace
	const FVector TraceStart = AvatarActor->GetActorLocation();
	const FVector TraceEnd   = TraceStart + AvatarActor->GetActorForwardVector() * AttackRange;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(AvatarActor);  // 자기 자신 제외

	TArray<FHitResult> HitResults;
	GetWorld()->SweepMultiByChannel(
		HitResults, TraceStart, TraceEnd, FQuat::Identity,
		ECC_Pawn, FCollisionShape::MakeSphere(AttackRadius), Params);

	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	if (!SourceASC) return;

	const float AttackPower = SourceASC->GetNumericAttribute(UDGAttributeSet::GetAttackPowerAttribute());

	// TSet으로 동일 액터 중복 타격 방지
	TSet<AActor*> HitActors;
	for (const FHitResult& Hit : HitResults)
	{
		AActor* HitActor = Hit.GetActor();
		if (!HitActor || HitActors.Contains(HitActor)) continue;
		HitActors.Add(HitActor);

		UAbilitySystemComponent* TargetASC =
			UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);
		if (!TargetASC) continue;

		FGameplayEffectContextHandle Context = SourceASC->MakeEffectContext();
		Context.AddInstigator(GetAvatarActorFromActorInfo(), GetAvatarActorFromActorInfo());

		FGameplayEffectSpecHandle Spec = SourceASC->MakeOutgoingSpec(DamageEffectClass, 1.f, Context);
		if (!Spec.IsValid()) continue;

		// SetByCaller로 데미지 값 주입 (GE에서 IncomingDamage에 적용)
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
			Spec, DGGameplayTags::SetByCaller_Damage_Base, AttackPower);

		const FActiveGameplayEffectHandle Applied =
			SourceASC->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), TargetASC);

		UE_LOG(LogDungeon, Log,
			TEXT("[GA_LightAttack] Hit: %s | Damage=%.1f | GE Applied=%s"),
			*HitActor->GetName(), AttackPower,
			Applied.IsValid() ? TEXT("YES") : TEXT("NO"));
	}

	if (HitResults.IsEmpty())
	{
		UE_LOG(LogDungeon, Log, TEXT("[GA_LightAttack] SphereTrace: 피격 대상 없음 (Range=%.0f, Radius=%.0f)"),
			AttackRange, AttackRadius);
	}
}

void UDGGameplayAbility_LightAttack::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UDGGameplayAbility_LightAttack::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}