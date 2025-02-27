#include "PlayerCharacterController.h"
#include "PlayerCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
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

void APlayerCharacterController::CloseMainMenu()
{
    // 게임 입력 모드로 변경
    FInputModeGameOnly GameMode;
    SetInputMode(GameMode);

    bShowMouseCursor = false;

    // 플레이어 입력 활성화
    SetIgnoreLookInput(false);
    SetIgnoreMoveInput(false);
}
