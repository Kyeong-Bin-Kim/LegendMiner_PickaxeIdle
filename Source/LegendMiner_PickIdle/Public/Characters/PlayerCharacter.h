#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

class AOre;
class UAnimMontage;

UCLASS()
class LEGENDMINER_PICKIDLE_API APlayerCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    APlayerCharacter();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable, Category = "Movement")
    void SetTargetLocation(const FVector& NewTargetLocation);

    UFUNCTION(BlueprintCallable, Category = "Pickaxe")
    float GetMiningSpeedBonus() const;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
    float Speed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float CameraDistance = 450.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    FVector CameraLotation = FVector(0.0f, 0.0f, 200.0f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    FRotator CameraRotation = FRotator(-30.0f, 0.0f, 0.0f);

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float AcceptableDistance = 50.f;

    FVector TargetLocation;
    bool bMovingToTarget;
    float CurrentSpeed;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    class USpringArmComponent* SpringArm;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickaxe")
	class UPickaxeComponent* PickaxeComponent;

private:
    AOre* TargetOre; // 가장 가까운 광석 저장
    bool bLookingAtOre;
    FTimerHandle LookAtOreTimer;
    FTimerHandle MiningTimer;

    void FindClosestOre();
    void RotateTowardsOre(float DeltaTime);
    void StartMining();
};
