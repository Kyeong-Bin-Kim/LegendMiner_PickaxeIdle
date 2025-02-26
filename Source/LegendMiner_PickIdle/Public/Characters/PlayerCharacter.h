#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class LEGENDMINER_PICKIDLE_API APlayerCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    APlayerCharacter();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // 이동 목표 위치 지정 (Blueprint에서 호출 가능)
    UFUNCTION(BlueprintCallable, Category = "Movement")
    void SetTargetLocation(const FVector& NewTargetLocation);

    // 애니메이션 블루프린트에 전달할 이동 속도
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
    float Speed;

    // 카메라 관련 설정
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float CameraDistance = 450.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    FVector CameraLotation = FVector(0.0f, 0.0f, 200.0f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    FRotator CameraRotation = FRotator(-30.0f, 0.0f, 0.0f);

protected:
    // 이동 관련 설정
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float AcceptableDistance = 10.f;

    // 이동 목표 위치
    FVector TargetLocation;

    // 이동 중인지 여부
    bool bMovingToTarget;

    // 내부 이동 속도 변수
    float CurrentSpeed;

    // 카메라 컴포넌트들
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    class USpringArmComponent* SpringArm;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    class UCameraComponent* Camera;
};
