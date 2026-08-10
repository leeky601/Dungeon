#include "Gameplay/Player/DGGameState.h"
#include "Features/Session/DGDungeonSessionComponent.h"

ADGGameState::ADGGameState()
{
	DungeonSessionComponent = CreateDefaultSubobject<UDGDungeonSessionComponent>(TEXT("DungeonSessionComponent"));
}

void ADGGameState::StartDungeonSession()
{
	DungeonSessionComponent->StartDungeonSession();
}

void ADGGameState::SetDungeonSessionState(EDGDungeonSessionState NewState)
{
	DungeonSessionComponent->SetDungeonSessionState(NewState);
}

void ADGGameState::StartBossEncounter()
{
	DungeonSessionComponent->StartBossEncounter();
}

void ADGGameState::MarkBossDefeated()
{
	DungeonSessionComponent->MarkBossDefeated();
}

void ADGGameState::EndDungeonSession()
{
	DungeonSessionComponent->EndDungeonSession();
}

EDGDungeonSessionState ADGGameState::GetDungeonSessionState() const
{
	return DungeonSessionComponent->GetDungeonSessionState();
}

bool ADGGameState::IsSessionInProgress() const
{
	return DungeonSessionComponent->IsSessionInProgress();
}

bool ADGGameState::IsBossStarted() const
{
	return DungeonSessionComponent->IsBossStarted();
}

bool ADGGameState::IsMatchEnded() const
{
	return DungeonSessionComponent->IsMatchEnded();
}