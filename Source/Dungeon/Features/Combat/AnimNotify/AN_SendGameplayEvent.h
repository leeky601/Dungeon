#pragma once

#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "AN_SendGameplayEvent.generated.h"

// AnimNotify: 설정된 GameplayTag를 오너 액터 ASC에 이벤트로 전달한다.
// 서버/클라이언트 양쪽에서 발동되며, 실제 히트 판정은 Authority 체크 후 GA 측에서 처리한다.
UCLASS(meta=(DisplayName="Send Gameplay Event"))
class DUNGEON_API UAN_SendGameplayEvent : public UAnimNotify
{
	GENERATED_BODY()

public:
	UAN_SendGameplayEvent();

	virtual void Notify(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;

	// 전송할 GameplayEvent 태그. 몽타주 에디터에서 히트 타이밍마다 설정한다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gameplay")
	FGameplayTag EventTag;
};