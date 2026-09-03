#include "Gameplay/Player/DGHubGameMode.h"
#include "Gameplay/Player/DGHubPlayerState.h"

ADGHubGameMode::ADGHubGameMode()
{
	PlayerStateClass = ADGHubPlayerState::StaticClass();
}
