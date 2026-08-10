#pragma once

#include "Components/ActorComponent.h"
#include "DGSessionProgressComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FDGOnSessionProgressChanged, int32, SessionLevel, int32, CurrentSessionXP, int32, XPToNextLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDGOnSessionLevelUp, int32, NewLevel, int32, OldLevel);

// 던전 세션 한정 Level/XP 진행도.
// 던전 입장 시 InitializeSessionProgress(), 종료 시 ResetSessionProgress()로 리셋된다.
// XP 지급은 서버 게임플레이 판정(몬스터 처치/PvP 처치/보스 기여도)에서 AddSessionXP()를 직접 호출해야 하며,
// 클라이언트가 XP 지급을 요청하는 RPC는 의도적으로 제공하지 않는다.
UCLASS(ClassGroup=(DG), meta=(BlueprintSpawnableComponent))
class DUNGEON_API UDGSessionProgressComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDGSessionProgressComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 던전 입장/세션 시작 시 Level 1 / XP 0으로 초기화. 서버 전용.
	void InitializeSessionProgress();

	// 던전 종료 시 Level 1 / XP 0으로 초기화. 서버 전용.
	void ResetSessionProgress();

	// 서버 게임플레이 판정(몬스터 처치, PvP 처치, 보스 기여도 등) 결과로만 호출한다. 서버 전용.
	// MaxSessionLevel 도달 후에는 XP를 누적하지 않는다.
	void AddSessionXP(int32 XPAmount);

	UFUNCTION(BlueprintPure, Category="Session")
	int32 GetSessionLevel() const { return SessionLevel; }

	UFUNCTION(BlueprintPure, Category="Session")
	int32 GetCurrentSessionXP() const { return CurrentSessionXP; }

	UFUNCTION(BlueprintPure, Category="Session")
	int32 GetXPToNextLevel() const { return XPToNextLevel; }

	UFUNCTION(BlueprintPure, Category="Session")
	bool IsMaxSessionLevel() const { return SessionLevel >= MaxSessionLevel; }

	// UI ProgressBar용 0~1 값. Max Level 도달 시 1.0 고정.
	UFUNCTION(BlueprintPure, Category="Session")
	float GetSessionXPProgressRatio() const;

	// 디버그용 XP 지급 테스트 함수. 내부적으로 AddSessionXP()를 호출하며 서버 권한이 없으면 무시하고 경고 로그만 남긴다.
	// Exec 미사용 이유: UE 콘솔 커맨드 라우팅은 PlayerController/Pawn/HUD/CheatManager만 스캔하고
	// PlayerState가 소유한 ActorComponent는 대상이 아니다. Blueprint(에디터 디버그 UI 등)에서 직접 호출한다.
	UFUNCTION(BlueprintCallable, Category="Session|Debug")
	void Debug_AddSessionXP(int32 XPAmount);

	// UI가 구독할 진행도 변경 알림. 서버 mutation 직후(서버) / OnRep(클라이언트) 양쪽에서 Broadcast된다.
	UPROPERTY(BlueprintAssignable, Category="Session")
	FDGOnSessionProgressChanged OnSessionProgressChanged;

	// 레벨이 실제로 상승했을 때만 Broadcast (ResetSessionProgress로 인한 하락 시에는 발생하지 않음).
	UPROPERTY(BlueprintAssignable, Category="Session")
	FDGOnSessionLevelUp OnSessionLevelUp;

private:
	// CurrentSessionXP >= XPToNextLevel인 동안 반복 레벨업 처리. MaxSessionLevel 이상으로는 올리지 않음.
	void TryLevelUp();

	static int32 CalculateXPToNextLevel(int32 CurrentLevel);

	void BroadcastProgressChanged();

	UFUNCTION() void OnRep_SessionLevel(int32 OldSessionLevel);
	UFUNCTION() void OnRep_CurrentSessionXP();
	UFUNCTION() void OnRep_XPToNextLevel();

	UPROPERTY(ReplicatedUsing=OnRep_SessionLevel)
	int32 SessionLevel = 1;

	UPROPERTY(ReplicatedUsing=OnRep_CurrentSessionXP)
	int32 CurrentSessionXP = 0;

	UPROPERTY(ReplicatedUsing=OnRep_XPToNextLevel)
	int32 XPToNextLevel = 100;

	UPROPERTY(EditDefaultsOnly, Category="Session")
	int32 MaxSessionLevel = 10;
};
