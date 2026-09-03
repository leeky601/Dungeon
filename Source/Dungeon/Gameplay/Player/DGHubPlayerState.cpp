#include "Gameplay/Player/DGHubPlayerState.h"
#include "Features/Inventory/DGHubStorageComponent.h"

ADGHubPlayerState::ADGHubPlayerState()
{
	HubStorageComponent = CreateDefaultSubobject<UDGHubStorageComponent>(TEXT("HubStorageComponent"));
}
