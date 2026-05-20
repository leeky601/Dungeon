#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "DGTypes.generated.h"

USTRUCT(BlueprintType)
struct FDGDamageData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DamageAmount = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag DamageTypeTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AActor> Instigator = nullptr;
};
