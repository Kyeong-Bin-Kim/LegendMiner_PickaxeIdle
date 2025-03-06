#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MessageWidget.h"
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
	// 메시지 위젯 클래스
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UMessageWidget> MessageWidgetClass;

    // 메인 메뉴 UI 위젯 클래스
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UMainMenuWidget> MainMenuWidgetClass;

public:
    /**
     * 메시지 위젯 생성 및 표시
     * @param Message - 표시할 메시지 텍스트
     * @param bShowCancelButton - 취소 버튼 표시 여부
     * @param CallbackListener - 콜백을 받을 객체 (해당 객체에 UFUNCTION이 있어야 함)
     * @param CallbackFunctionName - 콜백으로 바인딩할 UFUNCTION 이름
     * @param ConfirmText - 확인 버튼 텍스트 (기본값: "확인")
     * @param CancelText - 취소 버튼 텍스트 (기본값: "취소")
     */
    UFUNCTION(BlueprintCallable, Category = "UI")
    void ShowMessage(const FText& Message, bool bShowCancelButton, UObject* CallbackListener, FName CallbackFunctionName,
        const FText& ConfirmText = NSLOCTEXT("MessageWidget", "Confirm", "확인"),
        const FText& CancelText = NSLOCTEXT("MessageWidget", "Cancel", "취소"));

    // 메시지 확인 콜백 핸들러
    UFUNCTION()
    void OnMessageConfirmedHandler(bool bConfirmed);

    // 현재 활성화된 플레이어 인벤토리 UI 위젯
    UPROPERTY()
    UPlayerInventoryWidget* PlayerInventoryWidgetInstance;

    // 플레이어 인벤토리 정보를 표시할 UI 위젯 클래스
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UPlayerInventoryWidget> PlayerInventoryWidgetClass;

    // UI 갱신 함수
	UFUNCTION()
    void UpdateInventoryUI();

private:
    UPROPERTY()
    UMessageWidget* MessageWidgetInstance;

};
