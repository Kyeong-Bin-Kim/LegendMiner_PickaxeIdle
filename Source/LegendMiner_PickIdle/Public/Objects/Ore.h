#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OreData.h"
#include "Ore.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class AOreSpawner;
class APlayerCharacter;
class UPlayerSaveData;

UCLASS()
class LEGENDMINER_PICKIDLE_API AOre : public AActor
{
    GENERATED_BODY()

public:
    AOre();
    virtual void BeginPlay() override;

    // 광석 초기화
    UFUNCTION()
    void InitializeOre(int32 InOreLevel, AOreSpawner* InSpawner);

    // 채굴 시작
    UFUNCTION()
    void StartMining(APlayerCharacter* Player);

    UFUNCTION(BlueprintCallable, Category = "Ore")
    float GetMiningTime() const { return MiningTime; }

    // 채굴 주기 로직 (타이머에 의해 반복 호출)
    UFUNCTION()
    void MineOre();

    // 채굴 중단
    UFUNCTION()
    void StopMining();

    UFUNCTION()
    void RefreshSaveData();

    UPROPERTY(VisibleAnywhere, Category="Components")
    USphereComponent* OreTrigger;

private:
    UPROPERTY(VisibleAnywhere, Category="Components")
    UStaticMeshComponent* OreMesh;

    UPROPERTY()
    AOreSpawner* SpawnerRef;

    UPROPERTY()
    APlayerCharacter* PlayerRef;

    UPROPERTY()
    UPlayerSaveData* CachedSaveData;

    int32 OreLevel;
    float OreHealth;
    float MiningTime;

    FTimerHandle MiningTimerHandle;

    void UpdateOreAppearance();
    int32 GetMaxOreLevel() const;
    void DestroyOre();
};
