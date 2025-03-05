#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "OreInventoryItem.h"
#include "OreData.h"
#include "PickaxeData.h"
#include "Components/Border.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Components/ScaleBox.h"
#include "Components/Button.h"
#include "Engine/DataTable.h"
#include "PlayerInventoryWidget.generated.h"

/**
 * 플레이어가 보유한 광석을 표시하는 인벤토리 UI
 */
UCLASS()
class LEGENDMINER_PICKIDLE_API UPlayerInventoryWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION()
    void GenerateUI();

    UFUNCTION()
    void UpdateOreListBorderHeight();

    UFUNCTION()
    void UpdateInventoryList();

    UFUNCTION()
    void UpdatePickaxeUpgradeUI();

    UFUNCTION()
    void UpdateGold(int32 NewGoldAmount);

    UFUNCTION()
    void OnSellOreClicked();

    UFUNCTION()
    void OnAllSellOreClicked();

    UFUNCTION()
    void OnUpgradePickaxeClicked();

    UFUNCTION(BlueprintCallable, Category = "UI")
    void UpdateSingleOreQuantity(FName OreID, int32 NewQuantity);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    float OreListBorderWidth = 250.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    float OreListBorderHeight = 480.f;

    // 가로 박스 크기 조정 가능하도록 설정
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    float OreEntryWidth = 230.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    float OreEntryHeight = 35.f;

	// 아이콘 스케일 박스 크기 조정 가능하도록 설정
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    float OreIconScalePaddingSize = 10.f;

    // 아이콘 크기 조정 가능하도록 설정
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    float OreIconSize = 35.f;

    // 수량 텍스트 크기 조정 가능하도록 설정
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    int32 OreQuantityFontSize = 18;

protected:
    virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget))
	UBorder* OreListBorder;

    UPROPERTY(meta = (BindWidget))
    USizeBox* OreListSizeBox;

    UPROPERTY(meta = (BindWidget))
    UVerticalBox* OreListContainer;

    UPROPERTY(meta = (BindWidget))
    UBorder* PickaxeUpgradeBorder;

    UPROPERTY(meta = (BindWidget))
    UImage* UpgradeOreIcon;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* UpgradeOreQuantityText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* UpgradeMoneyQuantityText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    float ListPadding = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    float EntryPadding = 5.0f;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* GoldText;

    UPROPERTY(meta = (BindWidget))
    UButton* SellOreButton;

    UPROPERTY(meta = (BindWidget))
    UButton* AllSellOreButton;

    UPROPERTY(meta = (BindWidget))
    UButton* UpgradePickaxeButton;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* UpgradePickaxeButtonText;

    // 데이터 테이블 속성 추가 (블루프린트에서 설정 가능)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
    UDataTable* OreDataTable;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
    UDataTable* PickaxeDataTable;
};
