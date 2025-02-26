#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "OreInventoryItem.h"
#include "PlayerSaveData.generated.h"

UCLASS()
class LEGENDMINER_PICKIDLE_API UPlayerSaveData : public USaveGame
{
    GENERATED_BODY()

public:
    UPlayerSaveData();

    // 저장 슬롯 이름
    UPROPERTY(VisibleAnywhere, Category = "SaveData")
    FString SaveSlotName;

    // 사용자 인덱스 (기본값 0)
    UPROPERTY(VisibleAnywhere, Category = "SaveData")
    int32 UserIndex;

    // 플레이어의 곡괭이 레벨
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player Data")
    int32 PickaxeLevel;

    // 플레이어의 보유 골드
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player Data")
    int32 PlayerGold;

    // 플레이어가 채굴한 광석 인벤토리
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory")
    TArray<FOreInventoryItem> OreInventory;

    // 저장 데이터 초기화
    void InitializeSaveData();

    // 게임 데이터 저장
    void SaveGameData();

    // 게임 데이터 불러오기
    static UPlayerSaveData* LoadGameData();

    // 곡괭이 레벨 설정
    void SetPickaxeLevel(int32 NewLevel);

    // 곡괭이 레벨 반환
    int32 GetPickaxeLevel() const;

    // 골드 추가
    void AddGold(int32 Amount);

    // 현재 보유 골드 반환
    int32 GetPlayerGold() const;

    // 광석 인벤토리에 추가
    void AddOreToInventory(FName OreID, int32 Amount);

    // 특정 광석 개수 반환
    int32 GetOreQuantity(FName OreID) const;

    // 광석 판매 (골드 추가 후 광석 제거)
    void SellOre(FName OreID, int32 Amount, int32 GoldValue);
};
