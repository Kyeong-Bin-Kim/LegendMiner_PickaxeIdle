#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerInventoryWidget.h"
#include "PlayerCharacter.generated.h"

class AOre;
class UPickaxeComponent;

UCLASS()
class LEGENDMINER_PICKIDLE_API APlayerCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    APlayerCharacter();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // 이동할 타겟 위치 설정
    UFUNCTION(BlueprintCallable, Category = "Movement")
    void SetTargetLocation(const FVector& NewTargetLocation);

    // 채굴 보너스 (PickaxeComponent 등에서 가져옴)
    UFUNCTION(BlueprintCallable, Category = "Pickaxe")
    float GetMiningSpeedBonus() const;

    // 실제 채굴 시간
    UFUNCTION(BlueprintCallable, Category = "Pickaxe")
    float GetMiningSpeed() const;

    // 채굴 중단 시 애니메이션 정지
    UFUNCTION(BlueprintCallable, Category = "Pickaxe")
    void StopMining();

    UFUNCTION(BlueprintCallable, Category = "Pickaxe")
    UPickaxeComponent* GetPickaxeComponent() const;

protected:
    // 카메라 관련
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    class USpringArmComponent* SpringArm;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    class UCameraComponent* Camera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickaxe")
    UPickaxeComponent* PickaxeComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float AcceptableDistance = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float CameraDistance = 450.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    FVector CameraLotation = FVector(0.0f, 0.0f, 200.0f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    FRotator CameraRotation = FRotator(-30.0f, 0.0f, 0.0f);

    // 이동/채굴 관련 플래그
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mining")
    bool bMining;

public:
    UPROPERTY()
    UPlayerInventoryWidget* CachedInventoryWidget;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
    float Speed;

private:

    AOre* TargetOre;
    FVector TargetLocation;
    bool bMovingToTarget;
    float CurrentSpeed;
    bool bLookingAtOre;

    float CurrentMiningSpeed; // 필요에 따라 사용
    void FindClosestOre();
    void RotateTowardsOre(float DeltaTime);
    void StartMining();       // 오브젝트와 실제 채굴 로직 시작
};