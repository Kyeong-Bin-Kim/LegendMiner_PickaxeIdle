#include "InteractionComponent.h"

UInteractionComponent::UInteractionComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UInteractionComponent::BeginPlay()
{
    Super::BeginPlay();
}

// 플레이어가 곡괭이 레벨이 충분한지 확인하는 함수
bool UInteractionComponent::CanPlayerMine(int32 PlayerPickaxeLevel, int32 RequiredPickaxeLevel)
{
    return PlayerPickaxeLevel >= RequiredPickaxeLevel;
}
