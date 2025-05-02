#include "MainMenuWidget.h"
#include "PlayerSaveData.h"
#include "LegendMinerGameInstance.h"
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
    ULegendMinerGameInstance* GameInstance = Cast<ULegendMinerGameInstance>(GetGameInstance());

    if (GameInstance)
    {
        GameInstance->bMainMenuCloseInitialized = true;
    }

    this->RemoveFromParent();  // UI 제거
}


// 선택된 레벨로 이동하는 함수 추가
void UMainMenuWidget::OnStartGameClicked()
{
    bool bHasSaveData = UGameplayStatics::DoesSaveGameExist(TEXT("PlayerSaveSlot"), 0);

    // 기존 세이브 데이터가 있을 경우, 메시지를 먼저 띄움
    if (bHasSaveData)
    {
        ALegendMinerHUD* LegendMinerHUD = Cast<ALegendMinerHUD>(GetOwningPlayer()->GetHUD());
        if (LegendMinerHUD)
        {
            LegendMinerHUD->ShowMessage(
                FText::FromString(TEXT("기존 데이터를 삭제하고\n새 게임을 시작하시겠습니까?")),
                true,
                this,
                "OnNewGameConfirmed",
                FText::FromString(TEXT("네")),
                FText::FromString(TEXT("아니오"))
            );
        }
        return;  // 메시지를 띄운 후 바로 종료, 사용자의 응답을 기다림
    }

    // 기존 세이브 데이터가 없을 경우, 바로 새 게임 시작
    StartNewGame();
}

void UMainMenuWidget::OnNewGameConfirmed(bool bConfirmed)
{
    if (!bConfirmed)
    {
        UE_LOG(LogTemp, Warning, TEXT("New game canceled by user."));
        return;  // 사용자가 "아니오"를 선택하면 아무것도 하지 않음
    }

    // 기존 세이브 데이터 삭제
    if (UGameplayStatics::DoesSaveGameExist(TEXT("PlayerSaveSlot"), 0))
    {
        UGameplayStatics::DeleteGameInSlot(TEXT("PlayerSaveSlot"), 0);
    }

    // 새 게임 시작
    StartNewGame();
}

void UMainMenuWidget::StartNewGame()
{
    // 새로운 세이브 데이터 생성
    UPlayerSaveData* NewSaveData = NewObject<UPlayerSaveData>();
    if (NewSaveData)
    {
        NewSaveData->InitializeSaveData();
        NewSaveData->SaveGameData();  // 새 데이터 즉시 저장
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create new save data!"));
        return;
    }

    // UI 제거
    CloseMainMenu();

    // 선택된 레벨로 이동
    if (LevelToLoad.IsValid())
    {
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
