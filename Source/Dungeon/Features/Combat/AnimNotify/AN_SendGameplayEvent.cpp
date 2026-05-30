#include "Features/Combat/AnimNotify/AN_SendGameplayEvent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Core/Tags/DGGameplayTags.h"
#include "Dungeon.h"

UAN_SendGameplayEvent::UAN_SendGameplayEvent()
{
	// 기본값: 근접 공격 히트 타이밍 이벤트. 몽타주 에디터에서 개별 오버라이드 가능.
	EventTag = DGGameplayTags::Event_Combat_Hit;
}

void UAN_SendGameplayEvent::Notify(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	const bool bHasAuthority = Owner->HasAuthority();
	const bool bIsLocallyControlled = [Owner]() -> bool
	{
		if (const APawn* Pawn = Cast<APawn>(Owner)) return Pawn->IsLocallyControlled();
		return false;
	}();

	UE_LOG(LogDungeon, Log,
		TEXT("[AN_SendGameplayEvent] Tag=%s | Authority=%s | LocalControlled=%s | Actor=%s"),
		*EventTag.ToString(),
		bHasAuthority   ? TEXT("YES") : TEXT("NO"),
		bIsLocallyControlled ? TEXT("YES") : TEXT("NO"),
		*Owner->GetName());

	if (!EventTag.IsValid())
	{
		UE_LOG(LogDungeon, Warning, TEXT("[AN_SendGameplayEvent] EventTag이 유효하지 않습니다 — 이벤트 전송 생략"));
		return;
	}

	FGameplayEventData Payload;
	Payload.Instigator = Owner;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Owner, EventTag, Payload);
}