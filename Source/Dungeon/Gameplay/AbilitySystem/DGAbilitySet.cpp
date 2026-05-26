#include "Gameplay/AbilitySystem/DGAbilitySet.h"
#include "Gameplay/AbilitySystem/DGAbilitySystemComponent.h"
#include "Dungeon.h"

void UDGAbilitySet::GiveToAbilitySystem(UDGAbilitySystemComponent* ASC) const
{
	if (!ASC) return;

	// 방어적 이중 권한 체크 — Character의 HasAuthority 이후에도 내부에서 재확인한다.
	AActor* OwnerActor = ASC->GetOwnerActor();
	if (!OwnerActor || !OwnerActor->HasAuthority())
	{
		UE_LOG(LogDungeon, Warning, TEXT("[GAS] GiveToAbilitySystem: called without authority on %s"), *GetName());
		return;
	}

	for (const FDGAbilitySet_GameplayAbility& Entry : GrantedAbilities)
	{
		if (!Entry.AbilityClass) continue;

		FGameplayAbilitySpec Spec(Entry.AbilityClass, Entry.AbilityLevel);
		if (Entry.InputTag.IsValid())
		{
			// DynamicAbilityTags write path: 5.7에서 setter API가 없으므로 직접 접근
			PRAGMA_DISABLE_DEPRECATION_WARNINGS
			Spec.DynamicAbilityTags.AddTag(Entry.InputTag);
			PRAGMA_ENABLE_DEPRECATION_WARNINGS
		}
		ASC->GiveAbility(Spec);
	}
}

FPrimaryAssetId UDGAbilitySet::GetPrimaryAssetId() const
{
	return FPrimaryAssetId("AbilitySet", GetFName());
}