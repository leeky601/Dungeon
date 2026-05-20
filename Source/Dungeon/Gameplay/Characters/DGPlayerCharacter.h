#pragma once

#include "Gameplay/Characters/DGCharacterBase.h"
#include "DGPlayerCharacter.generated.h"

UCLASS()
class DUNGEON_API ADGPlayerCharacter : public ADGCharacterBase
{
	GENERATED_BODY()

public:
	ADGPlayerCharacter();

protected:
	virtual void PossessedBy(AController* NewController) override;   // 서버: PlayerState 준비 완료
	virtual void OnRep_PlayerState() override;                       // 클라이언트: PlayerState 복제 완료

private:
	void InitFromPlayerState();
};
