#include "Features/Session/DGDungeonSessionComponent.h"
#include "Features/Session/DGSessionProgressComponent.h"
#include "Features/Inventory/DGSessionInventoryComponent.h"
#include "Gameplay/Player/DGPlayerState.h"
#include "GameFramework/GameStateBase.h"
#include "Net/UnrealNetwork.h"
#include "Dungeon.h"

UDGDungeonSessionComponent::UDGDungeonSessionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UDGDungeonSessionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UDGDungeonSessionComponent, SessionState, COND_None, REPNOTIFY_Always);
}

void UDGDungeonSessionComponent::StartDungeonSession()
{
	AActor* Owner = GetOwner();
	if (!Owner || !Owner->HasAuthority())
	{
		UE_LOG(LogDungeon, Warning, TEXT("[Session] StartDungeonSession: called without authority on %s"), *GetNameSafe(Owner));
		return;
	}

	SetDungeonSessionState(EDGDungeonSessionState::InProgress);

	ForEachPlayerSessionProgress([](UDGSessionProgressComponent& ProgressComponent)
	{
		ProgressComponent.InitializeSessionProgress();
	});

	// Hub -> Dungeon Loadout 전달은 아직 연결되지 않았다 — 빈 Loadout으로 세션 인벤토리만 초기화한다.
	ForEachPlayerSessionInventory([](UDGSessionInventoryComponent& InventoryComponent)
	{
		InventoryComponent.InitializeSessionInventory(TArray<FDGSessionLoadoutEntry>());
	});
}

void UDGDungeonSessionComponent::SetDungeonSessionState(EDGDungeonSessionState NewState)
{
	AActor* Owner = GetOwner();
	if (!Owner || !Owner->HasAuthority())
	{
		UE_LOG(LogDungeon, Warning, TEXT("[Session] SetDungeonSessionState: called without authority on %s"), *GetNameSafe(Owner));
		return;
	}

	if (NewState == SessionState)
	{
		return;
	}

	const EDGDungeonSessionState OldState = SessionState;
	SessionState = NewState;

	UE_LOG(LogDungeon, Log, TEXT("[Session] SetDungeonSessionState: %d -> %d Owner=%s"),
		static_cast<int32>(OldState), static_cast<int32>(NewState), *Owner->GetName());

	OnDungeonSessionStateChanged.Broadcast(SessionState, OldState);
}

void UDGDungeonSessionComponent::StartBossEncounter()
{
	// 확장 지점: 실제 보스 스폰/보스 AI 트리거는 아직 연결하지 않았다.
	SetDungeonSessionState(EDGDungeonSessionState::BossStarted);
}

void UDGDungeonSessionComponent::MarkBossDefeated()
{
	SetDungeonSessionState(EDGDungeonSessionState::BossDefeated);

	// 확장 지점: PvP Disable, Reward 지급 로직은 여기서(또는 OnDungeonSessionStateChanged 구독을 통해) 연결될 예정이다.
}

void UDGDungeonSessionComponent::EndDungeonSession()
{
	AActor* Owner = GetOwner();
	if (!Owner || !Owner->HasAuthority())
	{
		UE_LOG(LogDungeon, Warning, TEXT("[Session] EndDungeonSession: called without authority on %s"), *GetNameSafe(Owner));
		return;
	}

	SetDungeonSessionState(EDGDungeonSessionState::MatchEnded);

	ForEachPlayerSessionProgress([](UDGSessionProgressComponent& ProgressComponent)
	{
		ProgressComponent.ResetSessionProgress();
	});

	ForEachPlayerSessionInventory([](UDGSessionInventoryComponent& InventoryComponent)
	{
		InventoryComponent.ResetSessionInventory();
	});
}

void UDGDungeonSessionComponent::ForEachPlayerSessionProgress(TFunctionRef<void(UDGSessionProgressComponent&)> Func)
{
	const AGameStateBase* GameState = Cast<AGameStateBase>(GetOwner());
	if (!GameState)
	{
		UE_LOG(LogDungeon, Warning, TEXT("[Session] ForEachPlayerSessionProgress: owner %s is not a GameStateBase"), *GetNameSafe(GetOwner()));
		return;
	}

	for (APlayerState* PlayerState : GameState->PlayerArray)
	{
		ADGPlayerState* DGPlayerState = Cast<ADGPlayerState>(PlayerState);
		if (!DGPlayerState)
		{
			continue;
		}

		if (UDGSessionProgressComponent* ProgressComponent = DGPlayerState->GetSessionProgressComponent())
		{
			Func(*ProgressComponent);
		}
	}
}

void UDGDungeonSessionComponent::ForEachPlayerSessionInventory(TFunctionRef<void(UDGSessionInventoryComponent&)> Func)
{
	const AGameStateBase* GameState = Cast<AGameStateBase>(GetOwner());
	if (!GameState)
	{
		UE_LOG(LogDungeon, Warning, TEXT("[Session] ForEachPlayerSessionInventory: owner %s is not a GameStateBase"), *GetNameSafe(GetOwner()));
		return;
	}

	for (APlayerState* PlayerState : GameState->PlayerArray)
	{
		ADGPlayerState* DGPlayerState = Cast<ADGPlayerState>(PlayerState);
		if (!DGPlayerState)
		{
			continue;
		}

		if (UDGSessionInventoryComponent* InventoryComponent = DGPlayerState->GetSessionInventoryComponent())
		{
			Func(*InventoryComponent);
		}
	}
}

void UDGDungeonSessionComponent::OnRep_SessionState(EDGDungeonSessionState OldState)
{
	OnDungeonSessionStateChanged.Broadcast(SessionState, OldState);
}