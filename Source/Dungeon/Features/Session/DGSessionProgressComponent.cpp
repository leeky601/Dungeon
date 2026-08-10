#include "Features/Session/DGSessionProgressComponent.h"
#include "Net/UnrealNetwork.h"
#include "Dungeon.h"

UDGSessionProgressComponent::UDGSessionProgressComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);

	XPToNextLevel = CalculateXPToNextLevel(SessionLevel);
}

void UDGSessionProgressComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UDGSessionProgressComponent, SessionLevel,     COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDGSessionProgressComponent, CurrentSessionXP, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UDGSessionProgressComponent, XPToNextLevel,    COND_None, REPNOTIFY_Always);
}

void UDGSessionProgressComponent::InitializeSessionProgress()
{
	AActor* Owner = GetOwner();
	if (!Owner || !Owner->HasAuthority())
	{
		UE_LOG(LogDungeon, Warning, TEXT("[Session] InitializeSessionProgress: called without authority on %s"), *GetNameSafe(Owner));
		return;
	}

	SessionLevel = 1;
	CurrentSessionXP = 0;
	XPToNextLevel = CalculateXPToNextLevel(SessionLevel);

	UE_LOG(LogDungeon, Log, TEXT("[Session] InitializeSessionProgress: Level=1, XP=0/%d Owner=%s"), XPToNextLevel, *Owner->GetName());

	BroadcastProgressChanged();
}

void UDGSessionProgressComponent::ResetSessionProgress()
{
	InitializeSessionProgress();
}

void UDGSessionProgressComponent::AddSessionXP(int32 XPAmount)
{
	AActor* Owner = GetOwner();
	if (!Owner || !Owner->HasAuthority())
	{
		UE_LOG(LogDungeon, Warning, TEXT("[Session] AddSessionXP: called without authority on %s"), *GetNameSafe(Owner));
		return;
	}

	if (XPAmount <= 0) return;

	if (SessionLevel >= MaxSessionLevel)
	{
		// Max Level 정책: 더 이상 XP를 누적하지 않는다.
		return;
	}

	CurrentSessionXP += XPAmount;
	TryLevelUp();

	UE_LOG(LogDungeon, Log, TEXT("[Session] AddSessionXP: +%d XP (Level=%d, XP=%d/%d) Owner=%s"),
		XPAmount, SessionLevel, CurrentSessionXP, XPToNextLevel, *Owner->GetName());

	BroadcastProgressChanged();
}

void UDGSessionProgressComponent::TryLevelUp()
{
	const int32 OldLevel = SessionLevel;

	while (SessionLevel < MaxSessionLevel && XPToNextLevel > 0 && CurrentSessionXP >= XPToNextLevel)
	{
		CurrentSessionXP -= XPToNextLevel;
		++SessionLevel;

		if (SessionLevel >= MaxSessionLevel)
		{
			// Max Level 정책: 남은 XP를 버리고 진행도를 고정한다.
			CurrentSessionXP = 0;
			XPToNextLevel = 0;
			break;
		}

		XPToNextLevel = CalculateXPToNextLevel(SessionLevel);
	}

	if (SessionLevel != OldLevel)
	{
		UE_LOG(LogDungeon, Log, TEXT("[Session] LevelUp: %d -> %d Owner=%s"), OldLevel, SessionLevel, *GetNameSafe(GetOwner()));
		OnSessionLevelUp.Broadcast(SessionLevel, OldLevel);
	}
}

int32 UDGSessionProgressComponent::CalculateXPToNextLevel(int32 CurrentLevel)
{
	// Lv1->2: 100, Lv2->3: 150, Lv3->4: 225 ... (Base * 1.5^(Level-1))
	// 추후 DataAsset/DataTable로 교체 시 이 함수만 대체하면 된다.
	constexpr int32 BaseXP = 100;
	constexpr float GrowthRate = 1.5f;
	return FMath::RoundToInt(BaseXP * FMath::Pow(GrowthRate, static_cast<float>(CurrentLevel - 1)));
}

float UDGSessionProgressComponent::GetSessionXPProgressRatio() const
{
	if (SessionLevel >= MaxSessionLevel || XPToNextLevel <= 0) return 1.f;

	return FMath::Clamp(static_cast<float>(CurrentSessionXP) / static_cast<float>(XPToNextLevel), 0.f, 1.f);
}

void UDGSessionProgressComponent::Debug_AddSessionXP(int32 XPAmount)
{
	AActor* Owner = GetOwner();
	if (!Owner || !Owner->HasAuthority())
	{
		UE_LOG(LogDungeon, Warning, TEXT("[Session] Debug_AddSessionXP는 서버 권한에서만 동작합니다. Owner=%s"), *GetNameSafe(Owner));
		return;
	}

	AddSessionXP(XPAmount);
}

void UDGSessionProgressComponent::BroadcastProgressChanged()
{
	OnSessionProgressChanged.Broadcast(SessionLevel, CurrentSessionXP, XPToNextLevel);
}

void UDGSessionProgressComponent::OnRep_SessionLevel(int32 OldSessionLevel)
{
	if (SessionLevel > OldSessionLevel)
	{
		OnSessionLevelUp.Broadcast(SessionLevel, OldSessionLevel);
	}

	BroadcastProgressChanged();
}

void UDGSessionProgressComponent::OnRep_CurrentSessionXP()
{
	BroadcastProgressChanged();
}

void UDGSessionProgressComponent::OnRep_XPToNextLevel()
{
	BroadcastProgressChanged();
}