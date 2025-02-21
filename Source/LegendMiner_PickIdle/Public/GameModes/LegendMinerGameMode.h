#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LegendMinerGameMode.generated.h"

UCLASS()
class LEGENDMINER_PICKIDLE_API ALegendMinerGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    ALegendMinerGameMode();

protected:
    virtual void BeginPlay() override;
};