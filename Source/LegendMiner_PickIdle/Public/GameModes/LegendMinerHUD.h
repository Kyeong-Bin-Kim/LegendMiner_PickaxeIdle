#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "LegendMinerHUD.generated.h"

UCLASS()
class LEGENDMINER_PICKIDLE_API ALegendMinerHUD : public AHUD
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;

protected:
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UMainMenuWidget> MainMenuWidgetClass;
};
