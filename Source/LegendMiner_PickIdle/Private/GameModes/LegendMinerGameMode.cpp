#include "LegendMinerGameMode.h"

ALegendMinerGameMode::ALegendMinerGameMode()
{

}

void ALegendMinerGameMode::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogTemp, Warning, TEXT("GameMode: 현재 사용 중인 GameMode는 %s 입니다."), *GetNameSafe(this));
}