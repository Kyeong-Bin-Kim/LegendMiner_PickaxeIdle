#include "MainMenuWidget.h"
#include "LegendMinerHUD.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

bool UMainMenuWidget::Initialize()
{
    bool Success = Super::Initialize();
    if (!Success) return false;

    if (StartGameButton)
    {

        StartGameButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnStartGameClicked);
    }

    if (ContinueGameButton)
    {
        ContinueGameButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnContinueGameClicked);
    }

    if (QuitGameButton)
    {
        QuitGameButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnQuitGameClicked);
    }

    return true;
}

void UMainMenuWidget::NativeConstruct()
{
    Super::NativeConstruct();

    UpdateUIBasedOnSaveData();
}

void UMainMenuWidget::UpdateUIBasedOnSaveData()
{
    bool bHasSaveData = UGameplayStatics::DoesSaveGameExist(TEXT("PlayerSaveSlot"), 0);

    if (bHasSaveData)
    {
        if (StartGameText) StartGameText->SetText(FText::FromString(TEXT("새 게임")));
        if (ContinueGameText) ContinueGameText->SetText(FText::FromString(TEXT("이어하기")));
        if (QuitGameText) QuitGameText->SetText(FText::FromString(TEXT("게임 종료")));

        if (ContinueGameButton) ContinueGameButton->SetVisibility(ESlateVisibility::Visible);
    }
    else
    {
        if (StartGameText) StartGameText->SetText(FText::FromString(TEXT("게임 시작")));
        if (QuitGameText) QuitGameText->SetText(FText::FromString(TEXT("게임 종료")));

        if (ContinueGameButton) ContinueGameButton->SetVisibility(ESlateVisibility::Collapsed);
    }
}

// 메인 메뉴 닫기 함수 추가
void UMainMenuWidget::CloseMainMenu()
{
    UE_LOG(LogTemp, Warning, TEXT("Closing Main Menu and switching to Game Input Mode"));

    this->RemoveFromViewport();  // UI 제거

    // 플레이어 컨트롤러에서 입력 모드 변경
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (PC)
    {

        UE_LOG(LogTemp, Warning, TEXT("Switched to Game Input Mode"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to get PlayerController!"));
    }
}


// 선택된 레벨로 이동하는 함수 추가
void UMainMenuWidget::OnStartGameClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("Start Game button clicked!"));

    bool bHasSaveData = UGameplayStatics::DoesSaveGameExist(TEXT("PlayerSaveSlot"), 0);

    if (bHasSaveData)
    {
        UE_LOG(LogTemp, Warning, TEXT("Deleting existing save data..."));
        UGameplayStatics::DeleteGameInSlot(TEXT("PlayerSaveSlot"), 0);
    }

    UE_LOG(LogTemp, Warning, TEXT("Calling CloseMainMenu()..."));
    CloseMainMenu();  // UI 제거 요청

    if (LevelToLoad.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("Loading level: %s"), *LevelToLoad->GetName());
        UGameplayStatics::OpenLevelBySoftObjectPtr(this, LevelToLoad);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("LevelToLoad is invalid!"));
    }
}

void UMainMenuWidget::OnContinueGameClicked()
{
    if (UGameplayStatics::DoesSaveGameExist(TEXT("PlayerSaveSlot"), 0))
    {
        CloseMainMenu();

        if (LevelToLoad.IsValid())
        {
            UGameplayStatics::OpenLevelBySoftObjectPtr(this, LevelToLoad);
        }
    }
}

void UMainMenuWidget::OnQuitGameClicked()
{
    UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, false);
}
