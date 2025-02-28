#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "LegendMinerHUD.generated.h"

class UPlayerInventoryWidget;
class UPlayerSaveData;

UCLASS()
class LEGENDMINER_PICKIDLE_API ALegendMinerHUD : public AHUD
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;

protected:
    // 메인 메뉴 UI 위젯 클래스
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UMainMenuWidget> MainMenuWidgetClass;

public:
    // 현재 활성화된 플레이어 인벤토리 UI 위젯
    UPROPERTY()
    UPlayerInventoryWidget* PlayerInventoryWidgetInstance;

    // 플레이어 인벤토리 정보를 표시할 UI 위젯 클래스
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UPlayerInventoryWidget> PlayerInventoryWidgetClass;

    // UI 갱신 함수
	UFUNCTION()
    void UpdateInventoryUI();

};
