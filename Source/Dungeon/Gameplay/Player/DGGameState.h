#pragma once

#include "GameFramework/GameStateBase.h"
#include "Core/Types/DGTypes.h"
#include "DGGameState.generated.h"

class UDGDungeonSessionComponent;

// 던전 세션(한 판)의 서버 권한 상태를 보유하고 모든 클라이언트에 복제한다.
// 상태 전이는 DungeonSessionComponent에 위임하며, 여기서는 GameMode/서버 로직이 쓰는 얇은 진입점만 제공한다.
// Dungeon은 자체 EDGDungeonSessionState로 세션 진행을 관리하므로 Unreal 기본 MatchState 흐름(AGameState)에
// 의존하지 않는다. ADGGameMode도 AGameModeBase를 상속하므로 AGameStateBase로 유지한다.
UCLASS()
class DUNGEON_API ADGGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	ADGGameState();

	UDGDungeonSessionComponent* GetDungeonSessionComponent() const { return DungeonSessionComponent; }

	// 서버 전용. 던전 세션 시작(-> InProgress) + 전체 PlayerState SessionProgress 초기화.
	void StartDungeonSession();

	// 서버 전용. 세션 상태를 직접 지정한다.
	void SetDungeonSessionState(EDGDungeonSessionState NewState);

	// 서버 전용. 보스방 진입(-> BossStarted).
	void StartBossEncounter();

	// 서버 전용. 보스 처치(-> BossDefeated).
	void MarkBossDefeated();

	// 서버 전용. 매치 종료(-> MatchEnded) + 전체 PlayerState SessionProgress 리셋.
	void EndDungeonSession();

	EDGDungeonSessionState GetDungeonSessionState() const;
	bool IsSessionInProgress() const;
	bool IsBossStarted() const;
	bool IsMatchEnded() const;

private:
	UPROPERTY(VisibleAnywhere, Category="Session")
	TObjectPtr<UDGDungeonSessionComponent> DungeonSessionComponent;
};