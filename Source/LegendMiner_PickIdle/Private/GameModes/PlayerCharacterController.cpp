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

    // 입력 매핑 컨텍스트 등록
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
            EnhancedInput->BindAction(ClickMoveAction, ETriggerEvent::Started, this, &APlayerCharacterController::OnClickMove);
        }

        if (ClickMoveAction)
        {
            EnhancedInput->BindAction(ESCAction, ETriggerEvent::Started, this, &APlayerCharacterController::OnESCPressed);
        }
    }
}

void APlayerCharacterController::OnClickMove(const FInputActionValue& Value)
{
    if (!CanClickToMove() || !CachedPlayerCharacter)
        return;

    FHitResult HitResult;
    GetHitResultUnderCursor(ECC_Visibility, false, HitResult);

    if (HitResult.bBlockingHit && CachedPlayerCharacter)
    {
        // 도착 시까지 클릭 입력 잠금
        SetCanClickToMove(false);

        // 캐릭터에게 클릭 위치 전달 → 해당 위치 기준 가장 가까운 광석으로 이동
        CachedPlayerCharacter->SetTargetLocation(HitResult.Location);

        // 클릭 마커 액터 생성
        SpawnClickMarkerAtLocation(HitResult.Location);
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

void APlayerCharacterController::SpawnClickMarkerAtLocation(const FVector& WorldLocation)
{
    if (!ClickMarkerActorClass) return;

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    FVector MarkerLocation = WorldLocation + FVector(0.f, 0.f, 2.f); // 살짝 띄워서 바닥에 묻히지 않게

    GetWorld()->SpawnActor<AClickMarkerActor>(
        ClickMarkerActorClass,
        MarkerLocation,
        FRotator::ZeroRotator,
        Params
    );
}
