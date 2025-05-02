#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "PlayerCharacterController.generated.h"

class UInputMappingContext;
class UInputAction;
class APlayerCharacter;
class AClickMarkerActor;

/**
 * 플레이어 입력을 처리하는 컨트롤러 클래스
 */
UCLASS()
class LEGENDMINER_PICKIDLE_API APlayerCharacterController : public APlayerController
{
    GENERATED_BODY()

public:
    APlayerCharacterController();

protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

    // 마우스 클릭 시 호출되는 함수
    UFUNCTION()
    void OnClickMove(const FInputActionValue& Value);

    // ESC 버튼을 눌렀을 때 호출되는 함수
    UFUNCTION()
    void OnESCPressed();

    // 메시지 위젯에서 확인 버튼을 눌렀을 때 실행되는 함수
    UFUNCTION()
    void OnExitConfirmed(bool bConfirmed);

public:
    // 클릭 이동 허용 여부 조회
    FORCEINLINE bool CanClickToMove() const { return bCanClickToMove_; }

    // 클릭 이동 허용 여부 설정
    void SetCanClickToMove(bool bEnabled) { bCanClickToMove_ = bEnabled; }

    UFUNCTION(BlueprintCallable)
    void SpawnClickMarkerAtLocation(const FVector& WorldLocation);

protected:
    // 컨트롤러가 소유한 플레이어 캐릭터를 캐싱
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Player")
    APlayerCharacter* CachedPlayerCharacter;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputMappingContext* InputMappingContext;

    // 클릭 마커 액터 클래스
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ClickMarker")
    TSubclassOf<AClickMarkerActor> ClickMarkerActorClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputAction* ClickMoveAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputAction* ESCAction;

private:
	// 클릭 이동을 할 수 있는지 여부
    bool bCanClickToMove_ = true;
};
