#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerCharacterAnimInstance.generated.h"

UCLASS()
class LEGENDMINER_PICKIDLE_API UPlayerCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;

protected:
	UFUNCTION()
	void AnimNotify_Notify_MineHit();
};
