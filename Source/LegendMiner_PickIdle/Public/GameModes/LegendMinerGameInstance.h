#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "LegendMinerGameInstance.generated.h"

UCLASS()
class LEGENDMINER_PICKIDLE_API ULegendMinerGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GameState")
    bool bIsMainMenuInitialized = false;  // 시작 메뉴 UI 생성 여부를 저장
};
