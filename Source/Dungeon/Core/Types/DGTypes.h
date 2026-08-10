#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "DGTypes.generated.h"

// 던전 한 판(세션)의 진행 상태. 서버(GameState 소유 DungeonSessionComponent)에서만 전이시킨다.
// Waiting -> InProgress -> BossStarted -> BossDefeated -> MatchEnded 순서를 의도한 선형 흐름으로 가정한다.
UENUM(BlueprintType)
enum class EDGDungeonSessionState : uint8
{
	Waiting,
	InProgress,
	BossStarted,
	BossDefeated,
	MatchEnded
};

USTRUCT(BlueprintType)
struct FDGDamageData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DamageAmount = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag DamageTypeTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AActor> Instigator = nullptr;
};
