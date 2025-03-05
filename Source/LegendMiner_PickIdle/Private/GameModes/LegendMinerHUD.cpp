#include "LegendMinerHUD.h"
#include "MainMenuWidget.h"
#include "PlayerInventoryWidget.h"
#include "LegendMinerGameInstance.h"
#include "PlayerSaveData.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"

void ALegendMinerHUD::BeginPlay()
{
    Super::BeginPlay();

    // GameInstance 가져오기
    ULegendMinerGameInstance* GameInstance = Cast<ULegendMinerGameInstance>(GetGameInstance());

    // MainMenuWidget이 닫힌 후 광석 UI 추가
    if (GameInstance && GameInstance->bIsMainMenuInitialized && GameInstance->bMainMenuCloseInitialized)
    {
        if (PlayerInventoryWidgetClass)
        {
            PlayerInventoryWidgetInstance = CreateWidget<UPlayerInventoryWidget>(GetWorld(), PlayerInventoryWidgetClass);
            if (PlayerInventoryWidgetInstance)
            {
                PlayerInventoryWidgetInstance->AddToViewport();

                UPlayerSaveData* PlayerSaveData = UPlayerSaveData::LoadGameData();
                if (PlayerSaveData)
                {
                    PlayerInventoryWidgetInstance->UpdateInventoryList();
                    PlayerInventoryWidgetInstance->UpdateGold(PlayerSaveData->GetPlayerGold());
                    PlayerInventoryWidgetInstance->UpdatePickaxeUpgradeUI();
                }
            }
        }
    }

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


    // PlayerSaveData 구독하여 UI 자동 갱신
    UPlayerSaveData* PlayerSaveData = UPlayerSaveData::LoadGameData();
    if (PlayerSaveData)
    {
        PlayerSaveData->OnOreDataUpdated.AddDynamic(this, &ALegendMinerHUD::UpdateInventoryUI);
    }
}

void ALegendMinerHUD::UpdateInventoryUI()
{
    if (!PlayerInventoryWidgetInstance) return;

    // 저장된 플레이어 데이터 불러오기
    UPlayerSaveData* PlayerSaveData = UPlayerSaveData::LoadGameData();
    if (!PlayerSaveData)
    {
        UE_LOG(LogTemp, Warning, TEXT("LegendMinerHUD: No save data found, skipping UI update."));
        return;
    }

    // UI 업데이트 (인벤토리 리스트 & 골드)
    PlayerInventoryWidgetInstance->UpdateInventoryList();  // 매개변수 제거
    PlayerInventoryWidgetInstance->UpdateGold(PlayerSaveData->GetPlayerGold());
}

