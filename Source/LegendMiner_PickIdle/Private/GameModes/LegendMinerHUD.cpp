#include "LegendMinerHUD.h"
#include "MainMenuWidget.h"
#include "LegendMinerGameInstance.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

void ALegendMinerHUD::BeginPlay()
{
    Super::BeginPlay();

    // GameInstance에서 bIsMainMenuInitialized 값 가져오기
    ULegendMinerGameInstance* GameInstance = Cast<ULegendMinerGameInstance>(GetGameInstance());

    if (GameInstance && GameInstance->bIsMainMenuInitialized)
    {
        UE_LOG(LogTemp, Warning, TEXT("HUD: MainMenuWidget is already initialized, skipping creation."));
        return;
    }

    if (!ensureAlways(MainMenuWidgetClass)) return;

    UMainMenuWidget* MainMenuWidget = CreateWidget<UMainMenuWidget>(GetWorld(), MainMenuWidgetClass);
    if (MainMenuWidget)
    {
        MainMenuWidget->AddToViewport();

        // UI 생성 후 GameInstance 값 변경
        if (GameInstance)
        {
            GameInstance->bIsMainMenuInitialized = true;
        }

        UE_LOG(LogTemp, Warning, TEXT("HUD: MainMenuWidget added to viewport!"));
    }
}
