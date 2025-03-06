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

void ALegendMinerHUD::ShowMessage(const FText& Message, bool bShowCancelButton, UObject* CallbackListener, FName CallbackFunctionName,
    const FText& ConfirmText, const FText& CancelText)
{

    if (!MessageWidgetClass || !CallbackListener)
    {
        return;
    }

    UMessageWidget* MessageWidget = CreateWidget<UMessageWidget>(GetWorld(), MessageWidgetClass);
    if (MessageWidget)
    {
        // 메시지 및 버튼 텍스트 설정 (기본값은 MessageWidget 내에서 NSLOCTEXT를 통해 지정됨)
        MessageWidget->SetupMessage(Message, bShowCancelButton, ConfirmText, CancelText);

        // FScriptDelegate를 사용하여 FName으로 UFUNCTION을 바인딩합니다.
        FScriptDelegate ScriptDelegate;
        ScriptDelegate.BindUFunction(CallbackListener, CallbackFunctionName);
        MessageWidget->OnMessageConfirmed.AddUnique(ScriptDelegate);

        MessageWidget->AddToViewport();
    }
}

void ALegendMinerHUD::OnMessageConfirmedHandler(bool bConfirmed)
{
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "OnMessageConfirmedHandler");

    if (MessageWidgetInstance)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "MessageWidgetInstance");
        MessageWidgetInstance = nullptr; // 안전하게 NULL로 설정하여 재사용 가능하도록 함.
    }

    if (bConfirmed)
    {
        // 확인 버튼을 클릭했을 때 실행할 추가 로직
    }
    else
    {
        // 취소 버튼을 클릭했을 때 실행할 추가 로직
        return;
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

