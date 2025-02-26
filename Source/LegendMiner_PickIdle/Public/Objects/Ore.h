#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OreData.h"
#include "Ore.generated.h"

class AOreSpawner;
class APlayerCharacter;
class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class LEGENDMINER_PICKIDLE_API AOre : public AActor
{
    GENERATED_BODY()

public:
    AOre();

protected:
    virtual void BeginPlay() override;

public:
    // 광석 초기화
    void InitializeOre(int32 InOreLevel, AOreSpawner* InSpawner);

    // 채굴 시작
    void StartMining(APlayerCharacter* Player);

    // 채굴 중단
    void StopMining();

    // 광석 채굴 로직
    void MineOre();

    // 광석이 파괴될 때 처리
    void DestroyOre();

    // 외형 업데이트
    void UpdateOreAppearance();

    UPROPERTY(VisibleAnywhere, Category = "Components")
    USphereComponent* OreTrigger;

private:
    int32 GetMaxOreLevel() const;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UStaticMeshComponent* OreMesh;


    UPROPERTY()
    AOreSpawner* SpawnerRef;

    UPROPERTY(EditAnywhere, Category = "Ore")
    int32 OreLevel;

    UPROPERTY(EditAnywhere, Category = "Ore")
    float OreHealth;

    // 채굴 주기 타이머
    FTimerHandle MiningTimerHandle;
};
