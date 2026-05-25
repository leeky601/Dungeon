#pragma once

#include "Components/ActorComponent.h"
#include "DGCombatComponent.generated.h"

UCLASS(ClassGroup=(DG), meta=(BlueprintSpawnableComponent))
class DUNGEON_API UDGCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDGCombatComponent();

	int32 GetComboCount() const { return ComboCount; }
	void ResetCombo();

private:
	int32 ComboCount = 0;
};
