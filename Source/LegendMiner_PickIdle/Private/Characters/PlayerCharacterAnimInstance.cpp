#include "PlayerCharacterAnimInstance.h"
#include "PlayerCharacter.h"
#include "Ore.h"

void UPlayerCharacterAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();
}

void UPlayerCharacterAnimInstance::AnimNotify_Notify_MineHit()
{
    APlayerCharacter* Player = Cast<APlayerCharacter>(TryGetPawnOwner());
    if (Player && Player->GetTargetOre())
    {
        Player->GetTargetOre()->MineOre();
    }
}