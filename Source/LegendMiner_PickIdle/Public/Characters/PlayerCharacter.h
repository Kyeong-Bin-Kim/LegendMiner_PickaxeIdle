#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerInventoryWidget.h"
#include "Components/WidgetComponent.h"
#include "FloatingMessageWidget.h"
#include "PlayerCharacter.generated.h"

class AOre;
class UPickaxeComponent;

/**
 * 광석 채굴을 담당하는 플레이어 캐릭터 클래스
 * - 클릭한 위치 기준 가장 가까운 광석으로 이동
 * - 도착 시 자동 회전 → 채굴 시작
 */
UCLASS()
class LEGENDMINER_PICKIDLE_API APlayerCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    APlayerCharacter();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // 클릭한 위치 전달 → 가장 가까운 광석을 찾고 그 위치로 이동
    UFUNCTION(BlueprintCallable, Category = "Movement")
    void SetTargetLocation(const FVector& NewTargetLocation);

    // 곡괭이로부터 채굴 속도 보너스를 반환
    UFUNCTION(BlueprintCallable, Category = "Pickaxe")
    float GetMiningSpeedBonus() const;

    // 현재 타겟 광석
    UFUNCTION(BlueprintCallable, Category = "Mining")
    AOre* GetTargetOre() const { return TargetOre; }

    // 현재 타겟 광석의 채굴 시간
    UFUNCTION(BlueprintCallable, Category = "Pickaxe")
    float GetMiningSpeed() const;

    // 채굴 중단 시 애니메이션 정지
    UFUNCTION(BlueprintCallable, Category = "Pickaxe")
    void StopMining();

    // 채굴 중단 후 일정 시간 후 재탐색 시도
    UFUNCTION(BlueprintCallable, Category = "Pickaxe")
    void StopMiningAndRestart();

    // Pickaxe 컴포넌트 참조 반환
    UFUNCTION(BlueprintCallable, Category = "Pickaxe")
    UPickaxeComponent* GetPickaxeComponent() const;

protected:
    // 카메라 관련
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    class USpringArmComponent* PlayerSpringArm;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    class UCameraComponent* Camera;

    // 캐릭터 장비
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickaxe")
    UPickaxeComponent* PickaxeComponent;

	// 메시지 위젯 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
    UWidgetComponent* MessageWidgetComponent;

	// 메시지 위젯 인스턴스
    UFloatingMessageWidget* FloatingWidgetInstance;

    // 도착 허용 오차 거리 (채굴 위치 도달 판정)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float AcceptableDistance = 10.f;

    // 광석 탐색 반경 허용 범위
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mining")
    float OreSearchRadius = 150.f;

    // 카메라 위치 설정
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float CameraDistance = 450.f;

	// 카메라 위치 설정
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    FVector CameraLotation = FVector(0.0f, 0.0f, 200.0f);

	// 카메라 회전 설정
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    FRotator CameraRotation = FRotator(-30.0f, 0.0f, 0.0f);

    // 회전이 생략 가능한 각도 범위
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mining")
    float AcceptableLookAngle = 5.0f;

	// 이동 시작 시간
    float MoveStartTime = 0.f;

	// 최대 이동 시간
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float MaxMoveDuration = 3.f;

    // 이동/채굴 관련 플래그
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mining")
    bool bMining;

public:
    // 캐시된 인벤토리 위젯 (HUD 연동용)
    UPROPERTY()
    UPlayerInventoryWidget* CachedInventoryWidget;

    // 현재 이동 속도
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
    float Speed;

private:
    AOre* TargetOre;            // 현재 타겟 광석
    FVector TargetLocation;     // 이동 목표 지점
    bool bMovingToTarget;       // 이동 중 여부
    float CurrentSpeed;         // 가속된 이동 속도
    bool bLookingAtOre;         // 광석 바라보는 중 여부
    float CurrentMiningSpeed;   // 현재 채굴 속도

    // 가장 가까운 광석 탐색
    void FindClosestOre();
        
    // 주어진 위치 기준 가장 가까운 광석 탐색
    AOre* FindClosestOreFromPoint(const FVector& SearchOrigin);

    // 광석을 바라보도록 회전 처리
    void RotateTowardsOre(float DeltaTime);

	// 메시지 위젯을 생성하고 설정
    void ShowFloatingMessage(const FText& Message);

	// 메시지 위젯을 숨김
    void HideFloatingMessage();

    // 채굴 로직 시작
    void StartMining();

    // 일정 시간 후 다시 채굴을 시도할 타이머 핸들
    FTimerHandle FindOreTimerHandle;
};