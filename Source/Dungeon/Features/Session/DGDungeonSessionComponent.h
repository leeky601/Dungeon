#pragma once

#include "Components/ActorComponent.h"
#include "Templates/Function.h"
#include "Core/Types/DGTypes.h"
#include "DGDungeonSessionComponent.generated.h"

class UDGSessionProgressComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDGOnDungeonSessionStateChanged, EDGDungeonSessionState, NewState, EDGDungeonSessionState, OldState);

// 던전 한 판(세션) 진행 상태를 관리한다. GameState가 소유하며 모든 클라이언트에 복제된다.
// 상태 변경은 서버(GameMode 또는 서버 게임 로직)에서만 호출해야 하며, 클라이언트 요청 RPC는 의도적으로 제공하지 않는다.
UCLASS(ClassGroup=(DG), meta=(BlueprintSpawnableComponent))
class DUNGEON_API UDGDungeonSessionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDGDungeonSessionComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 던전 세션을 시작한다(-> InProgress). 소유 GameState의 PlayerArray를 순회하며
	// 각 PlayerState의 SessionProgressComponent를 InitializeSessionProgress()로 초기화한다. 서버 전용.
	void StartDungeonSession();

	// 세션 상태를 직접 지정한다. 특수한 전이가 필요할 때만 사용하고,
	// 일반적인 경우 StartDungeonSession()/StartBossEncounter()/MarkBossDefeated()/EndDungeonSession()을 사용한다. 서버 전용.
	void SetDungeonSessionState(EDGDungeonSessionState NewState);

	// 보스방 진입 시점 상태 전이(-> BossStarted). 서버 전용.
	// 확장 지점: 실제 보스 스폰/보스 AI 트리거는 아직 연결하지 않았다. 추후 보스 시스템에서 이 함수를 호출한다.
	void StartBossEncounter();

	// 보스 처치 시점 상태 전이(-> BossDefeated). 서버 전용.
	// 확장 지점: PvP Disable, Reward 지급 로직은 이 호출 이후(또는 이 함수 내부)에 연결될 예정이다.
	void MarkBossDefeated();

	// 매치 종료(-> MatchEnded). 소유 GameState의 PlayerArray를 순회하며
	// 각 PlayerState의 SessionProgressComponent를 ResetSessionProgress()로 초기화한다. 서버 전용.
	void EndDungeonSession();

	UFUNCTION(BlueprintPure, Category="Session")
	EDGDungeonSessionState GetDungeonSessionState() const { return SessionState; }

	UFUNCTION(BlueprintPure, Category="Session")
	bool IsSessionInProgress() const { return SessionState == EDGDungeonSessionState::InProgress; }

	UFUNCTION(BlueprintPure, Category="Session")
	bool IsBossStarted() const { return SessionState == EDGDungeonSessionState::BossStarted; }

	UFUNCTION(BlueprintPure, Category="Session")
	bool IsMatchEnded() const { return SessionState == EDGDungeonSessionState::MatchEnded; }

	// UI/시스템이 구독할 상태 변경 알림. 서버 mutation 직후(서버) / OnRep(클라이언트) 양쪽에서 Broadcast된다.
	UPROPERTY(BlueprintAssignable, Category="Session")
	FDGOnDungeonSessionStateChanged OnDungeonSessionStateChanged;

private:
	// 소유 GameState의 PlayerArray를 순회하며 각 ADGPlayerState의 SessionProgressComponent에 Func를 적용한다.
	void ForEachPlayerSessionProgress(TFunctionRef<void(UDGSessionProgressComponent&)> Func);

	UFUNCTION() void OnRep_SessionState(EDGDungeonSessionState OldState);

	UPROPERTY(ReplicatedUsing=OnRep_SessionState)
	EDGDungeonSessionState SessionState = EDGDungeonSessionState::Waiting;
};