#include "PlayerInventoryWidget.h"
#include "PlayerSaveData.h"
#include "Kismet/GameplayStatics.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/ScaleBoxSlot.h"
#include "Components/TextBlock.h"

void UPlayerInventoryWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // 버튼 클릭 이벤트 바인딩 (판매 기능 추가)
    if (SellOreButton)
    {
        SellOreButton->OnClicked.AddDynamic(this, &UPlayerInventoryWidget::OnSellOreClicked);
    }

    // 초기 금화 및 광석 UI 생성
    GenerateUI();
}

void UPlayerInventoryWidget::GenerateUI()
{
    if (!OreDataTable || !OreListContainer || !OreListBorder) return;

    OreListContainer->ClearChildren();

    TArray<FName> RowNames = OreDataTable->GetRowNames();
    for (const FName& RowName : RowNames)
    {
        FOreData* OreData = OreDataTable->FindRow<FOreData>(RowName, TEXT("Ore Lookup"));
        if (!OreData) continue;

        // 개별 가로 박스 크기 설정 (SizeBox)
        USizeBox* EntrySizeBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
        EntrySizeBox->SetWidthOverride(OreEntryWidth);
        EntrySizeBox->SetHeightOverride(OreEntryHeight);

        // 가로 정렬을 위한 Horizontal Box 생성
        UHorizontalBox* OreEntryBox = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());

        // 아이콘 크기 조정을 위한 ScaleBox 생성
        UScaleBox* OreIconScaleBox = WidgetTree->ConstructWidget<UScaleBox>(UScaleBox::StaticClass());

        // 아이콘 추가
        UImage* OreIcon = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
        OreIcon->SetBrushFromTexture(OreData->Icon);
        OreIcon->SetDesiredSizeOverride(FVector2D(OreIconSize, OreIconSize)); // 아이콘 크기 적용

        // ScaleBox 안에 Icon 추가
        UScaleBoxSlot* ScaleBoxSlot = Cast<UScaleBoxSlot>(OreIconScaleBox->AddChild(OreIcon));

        // 아이콘과 텍스트 사이 간격을 위한 SizeBox 추가
        USizeBox* SpacerSizeBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
        SpacerSizeBox->SetWidthOverride(OreIconScalePaddingSize);

        // 광석 개수 텍스트 추가 (기본값 0)
        UTextBlock* OreQuantityText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), *RowName.ToString());
        OreQuantityText->SetText(FText::AsNumber(0));
        OreQuantityText->SetFont(FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), OreQuantityFontSize)); // ✅ 글꼴 크기 적용
		OreQuantityText->SetLineHeightPercentage(0.9f); // 줄 간격 설정

        // UI 요소 배치
        OreEntryBox->AddChildToHorizontalBox(OreIconScaleBox);
        OreEntryBox->AddChildToHorizontalBox(SpacerSizeBox);
        OreEntryBox->AddChildToHorizontalBox(OreQuantityText);

        EntrySizeBox->AddChild(OreEntryBox);

        // 리스트에 추가
        OreListContainer->AddChild(EntrySizeBox);
    }

    UpdateOreListBorderHeight(); // 새로운 크기 조절 함수 호출
}

void UPlayerInventoryWidget::UpdateOreListBorderHeight()
{
    if (!OreListContainer || !OreListSizeBox) return;

    // 현재 리스트에 추가된 아이템 수 확인
    int32 NumEntries = OreListContainer->GetChildrenCount();
    if (NumEntries == 0) return;

    // 개별 가로 박스 높이와 패딩 값 고려하여 총 높이 계산
    float TotalHeight = (OreEntryHeight + (EntryPadding*2)) * NumEntries;

    // OreListSizeBox의 높이를 패딩을 포함하여 설정
    OreListSizeBox->SetHeightOverride(TotalHeight + ListPadding);

    UE_LOG(LogTemp, Warning, TEXT("OreListSizeBox height set to: %.2f"), TotalHeight + ListPadding);
}

void UPlayerInventoryWidget::UpdateInventoryList()
{
    if (!OreDataTable) return;

    UPlayerSaveData* PlayerSaveData = Cast<UPlayerSaveData>(UGameplayStatics::LoadGameFromSlot(TEXT("PlayerSaveSlot"), 0));
    if (!PlayerSaveData)
    {
        UE_LOG(LogTemp, Warning, TEXT("PlayerInventoryWidget: No save data found, skipping UI update."));
        return;
    }

    // 저장된 데이터가 있다면 UI 생성 후 즉시 반영
    TArray<FName> RowNames = OreDataTable->GetRowNames();
    for (const FName& RowName : RowNames)
    {
        UTextBlock* OreQuantityText = Cast<UTextBlock>(WidgetTree->FindWidget(*RowName.ToString()));
        if (!OreQuantityText) continue;

        int32 NewQuantity = 0;
        for (const FOreInventoryItem& OreItem : PlayerSaveData->OreInventory)
        {
            if (OreItem.OreID == RowName)
            {
                NewQuantity = OreItem.Quantity;
                break;
            }
        }

        OreQuantityText->SetText(FText::AsNumber(NewQuantity));
    }

    UpdateGold(PlayerSaveData->GetPlayerGold());
}

void UPlayerInventoryWidget::UpdateGold(int32 NewGoldAmount)
{
    if (!GoldText) return;

    GoldText->SetText(FText::AsNumber(NewGoldAmount));
}

void UPlayerInventoryWidget::OnSellOreClicked()
{
    UPlayerSaveData* PlayerSaveData = Cast<UPlayerSaveData>(UGameplayStatics::LoadGameFromSlot(TEXT("PlayerSaveSlot"), 0));
    if (!PlayerSaveData)
    {
        UE_LOG(LogTemp, Warning, TEXT("PlayerInventoryWidget: No save data found, cannot sell ores."));
        return;
    }

    int32 PickaxeLevel = PlayerSaveData->GetPickaxeLevel(); // 현재 플레이어 곡괭이 레벨 가져오기
    bool bHasSellableOres = false;

    for (const FOreInventoryItem& OreItem : PlayerSaveData->OreInventory)
    {
        int32 OreLevel = FCString::Atoi(*OreItem.OreID.ToString());

        // 판매 조건: 광석 레벨이 (곡괭이 레벨 - 1) 이하이거나, 곡괭이 레벨이 1일 경우 레벨 1 광석은 판매 가능
        if (!(OreLevel <= PickaxeLevel - 1 || (PickaxeLevel == 1 && OreLevel == 1)))
        {
            continue; // 판매 불가능한 광석은 건너뜀
        }

        FOreData* OreData = OreDataTable->FindRow<FOreData>(OreItem.OreID, TEXT("Ore Lookup"));
        if (!OreData) continue;

        bHasSellableOres = true;
        int32 SellPrice = OreData->Rarity; // 개당 가격을 설정
        PlayerSaveData->SellOre(OreItem.OreID, SellPrice); // 모든 개수 판매
    }

    if (!bHasSellableOres)
    {
        UE_LOG(LogTemp, Warning, TEXT("PlayerInventoryWidget: No ores available for sale."));
        return;
    }

	UpdateInventoryList(); // 판매 후 UI 업데이트
}

void UPlayerInventoryWidget::UpdateSingleOreQuantity(FName OreID, int32 NewQuantity)
{
    if (!OreDataTable) return;

    // 세이브 데이터에서 개수를 가져와 UI 업데이트
    UTextBlock* OreQuantityText = Cast<UTextBlock>(WidgetTree->FindWidget(*OreID.ToString()));
    if (!OreQuantityText)
    {
        UE_LOG(LogTemp, Warning, TEXT("PlayerInventoryWidget: OreID %s not found in UI!"), *OreID.ToString());
        return;
    }

    OreQuantityText->SetText(FText::AsNumber(NewQuantity));

    UE_LOG(LogTemp, Warning, TEXT("PlayerInventoryWidget: Updated OreID %s to %d"), *OreID.ToString(), NewQuantity);
}

