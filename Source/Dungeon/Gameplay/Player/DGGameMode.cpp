#include "Gameplay/Player/DGGameMode.h"
#include "Gameplay/Player/DGGameState.h"

void ADGGameMode::BeginPlay()
{
	Super::BeginPlay();

	StartDungeonSessionFromGameMode();
}

void ADGGameMode::StartDungeonSessionFromGameMode()
{
	if (ADGGameState* DGGameState = GetGameState<ADGGameState>())
	{
		DGGameState->StartDungeonSession();
	}
}