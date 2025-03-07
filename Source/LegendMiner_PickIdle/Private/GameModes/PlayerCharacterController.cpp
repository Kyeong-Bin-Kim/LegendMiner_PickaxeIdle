#include "PlayerCharacterController.h"
#include "PlayerCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "LegendMinerHUD.h"
#include "LegendMinerGameInstance.h"
#include "PlayerSaveData.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/World.h"

APlayerCharacterController::APlayerCharacterController()
    : CachedPlayerCharacter(nullptr)
{
}

void APlayerCharacterController::BeginPlay()
{
    Super::BeginPlay();
    bShowMouseCursor = true;

    // 소유한 Pawn을 캐싱합니다.
    APawn* PlayerPawn = GetPawn();
    if (PlayerPawn)
    {
        CachedPlayerCharacter = Cast<APlayerCharacter>(PlayerPawn);
    }

    if (InputMappingContext)
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(InputMappingContext, 0);
        }
    }
}

void APlayerCharacterController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
    {
        if (ClickMoveAction)
        {
            EnhancedInput->BindAction(ClickMoveAction, ETriggerEvent::Triggered, this, &APlayerCharacterController::OnClickMove);
        }

        if (ClickMoveAction)
        {
            EnhancedInput->BindAction(ESCAction, ETriggerEvent::Started, this, &APlayerCharacterController::OnESCPressed);
        }
    }
}

void APlayerCharacterController::OnClickMove(const FInputActionValue& Value)
{
    FHitResult HitResult;
    GetHitResultUnderCursor(ECC_Visibility, false, HitResult);

    if (HitResult.bBlockingHit && CachedPlayerCharacter)
    {
        CachedPlayerCharacter->SetTargetLocation(HitResult.Location);
    }
}

void APlayerCharacterController::OnESCPressed()
{
    ULegendMinerGameInstance* GameInstance = Cast<ULegendMinerGameInstance>(GetGameInstance());
    if (GameInstance)
    {
        // 메뉴가 한 번 생성되고 닫혔을 때만 ESC 키가 동작
        if (!GameInstance->bIsMainMenuInitialized || !GameInstance->bMainMenuCloseInitialized)
        {
            return;
        }
    }

    ALegendMinerHUD* LegendMinerHUD = Cast<ALegendMinerHUD>(GetHUD());
    if (LegendMinerHUD)
    {
        LegendMinerHUD->ShowMessage(
            FText::FromString(TEXT("정말 종료하시겠습니까?")),
            true,
            this,
            "OnExitConfirmed",
            FText::FromString(TEXT("확인")),
            FText::FromString(TEXT("취소"))
        );
    }
}

void APlayerCharacterController::OnExitConfirmed(bool bConfirmed)
{
    // 종료 전에 데이터 저장
    UPlayerSaveData* PlayerSaveData = UPlayerSaveData::LoadGameData();
    if (PlayerSaveData)
    {
        PlayerSaveData->SaveGameData(); // 저장 실행
        UE_LOG(LogTemp, Warning, TEXT("Game data saved before exit."));
    }

    // 게임 종료 실행
    UKismetSystemLibrary::QuitGame(GWorld, nullptr, EQuitPreference::Quit, false);
}