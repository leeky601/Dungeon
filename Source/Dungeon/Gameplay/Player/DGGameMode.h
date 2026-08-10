#pragma once

#include "GameFramework/GameModeBase.h"
#include "DGGameMode.generated.h"

// 서버 전용. 던전 세션 상태 변경 명령의 시작점 중 하나.
// 실제 상태 저장/복제는 ADGGameState(DungeonSessionComponent)가 담당한다.
// Dungeon은 자체 EDGDungeonSessionState로 세션 진행을 관리하므로 Unreal 기본 MatchState 머신(AGameMode)이
// 필요 없다. AGameStateBase 기반 ADGGameState와 조합하기 위해 AGameModeBase를 상속한다.
UCLASS()
class DUNGEON_API ADGGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	// 서버 전용. 던전 세션을 시작한다.
	// 현재는 BeginPlay에서 즉시 호출하지만(테스트 맵 기준), 추후 Hub/Ready/Matchmaking 단계가 추가되면
	// BeginPlay 자동 시작 대신 해당 단계의 완료 시점(예: 전원 Ready RPC 수신)에서 이 함수를 호출하도록
	// 호출 지점만 옮기면 된다.
	void StartDungeonSessionFromGameMode();
};