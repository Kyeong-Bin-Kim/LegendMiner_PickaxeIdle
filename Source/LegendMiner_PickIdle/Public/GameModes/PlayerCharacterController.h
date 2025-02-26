#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "PlayerCharacterController.generated.h"

// 전방 선언을 활용하여 불필요한 헤더 포함을 줄입니다.
class UInputMappingContext;
class UInputAction;
class APlayerCharacter;

UCLASS()
class LEGENDMINER_PICKIDLE_API APlayerCharacterController : public APlayerController
{
    GENERATED_BODY()

public:
    APlayerCharacterController();

protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

    // 컨트롤러가 소유한 플레이어 캐릭터를 캐싱합니다.
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Player")
    APlayerCharacter* CachedPlayerCharacter;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputMappingContext* InputMappingContext;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputAction* ClickMoveAction;

    UFUNCTION()
    void OnClickMove(const FInputActionValue& Value);
};
