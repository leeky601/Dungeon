#pragma once

#include "GameFramework/GameModeBase.h"
#include "DGHubGameMode.generated.h"

// Hub(로비) 서버 전용 GameMode. ADGHubPlayerState를 스폰하도록 PlayerStateClass만 지정한다.
// Hub는 아직 Unreal 기본 MatchState 상태머신이 필요 없어 AGameMode가 아닌 AGameModeBase를
// 상속한다(ADGGameMode와 동일한 근거 — DGGameMode.h 참고). GameStateClass는 기본값
// (AGameStateBase)을 그대로 사용한다.
UCLASS()
class DUNGEON_API ADGHubGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ADGHubGameMode();
};
