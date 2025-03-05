#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OreSpawner.generated.h"

class AOre;
class UDataTable;
class UPlayerSaveData;
class UBoxComponent;

UCLASS()
class LEGENDMINER_PICKIDLE_API AOreSpawner : public AActor
{
    GENERATED_BODY()

public:
    AOreSpawner();

protected:
    virtual void BeginPlay() override;

    // NavMesh 업데이트
    void UpdateNavMesh();

private:
    // 박스 콜리전 찾아서 등록
    void FindSpawnBoxes();

    // 플레이어의 곡괭이 레벨을 가져오는 함수
    int32 GetPlayerPickaxeLevel();

    // 광석 초기 스폰
    void SpawnInitialOres();

    // 박스 콜리전 내부에서 랜덤한 위치 반환
    FVector GetRandomSpawnLocation();

    // 최대 광석 레벨 반환 
    int32 GetMaxOreLevel(int32 PlayerPickaxeLevel);

    // 플레이어 곡괭이 레벨 이하의 랜덤 광석 레벨 반환
    int32 GetRandomOreLevel(int32 PlayerPickaxeLevel);

public:
    // 데이터 테이블: 광석 정보를 저장하는 데이터 테이블
    UPROPERTY(EditAnywhere, Category = "Ore")
    UDataTable* OreDataTable;

    // 특정 광석이 파괴되었을 때 새로운 광석을 생성
    void ReplaceOre(AOre* DestroyedOre);

private:

    // 플레이어 세이브 데이터: 곡괭이 레벨 확인을 위해 사용
    UPROPERTY(EditAnywhere, Category = "SaveData")
    UPlayerSaveData* PlayerSaveData;

    // 최대 광석 개수
    UPROPERTY(EditAnywhere, Category = "Spawning")
    int32 MaxOreCount = 50;

    // 스폰할 수 있는 박스 콜리전들
    TArray<UBoxComponent*> SpawnBoxes;

    // 현재 활성화된 광석 목록
    TArray<AOre*> ActiveOres;
};
