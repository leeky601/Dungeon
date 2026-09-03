#pragma once

#include "GameFramework/PlayerState.h"
#include "DGHubPlayerState.generated.h"

class UDGHubStorageComponent;

// Hub(로비) 서버 전용 PlayerState. UDGHubStorageComponent만 소유하며,
// Dungeon 서버 전용인 ADGPlayerState(ASC/AttributeSet/SessionProgress)와는 별개 클래스다.
// 서버 토폴로지/책임 분리 근거는 06-inventory-hub-rules.md 참고.
UCLASS()
class DUNGEON_API ADGHubPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ADGHubPlayerState();
	
	UFUNCTION(BlueprintPure, Category="Inventory")
	UDGHubStorageComponent* GetHubStorageComponent() const { return HubStorageComponent; }

private:
	UPROPERTY(VisibleAnywhere, Category="Inventory")
	TObjectPtr<UDGHubStorageComponent> HubStorageComponent;
};
