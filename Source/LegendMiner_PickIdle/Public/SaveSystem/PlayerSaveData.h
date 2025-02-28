#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "OreInventoryItem.h"
#include "PlayerSaveData.generated.h"

// UI 갱신을 위한 델리게이트 선언
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOreDataUpdated);

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

    // UI가 이를 구독하여 광석 데이터가 변경될 때 자동으로 업데이트할 수 있도록 이벤트 추가
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnOreDataUpdated OnOreDataUpdated;

    // 저장 데이터 초기화
    UFUNCTION()
    void InitializeSaveData();

    // 게임 데이터 저장
    UFUNCTION()
    void SaveGameData();

    // 게임 데이터 불러오기 (GameInstance에서 관리)
    UFUNCTION()
    static UPlayerSaveData* LoadGameData();

    // 곡괭이 레벨 설정
    UFUNCTION()
    void SetPickaxeLevel(int32 NewLevel);

    // 곡괭이 레벨 반환
    UFUNCTION()
    int32 GetPickaxeLevel() const;

    // 골드 추가
    UFUNCTION()
    void AddGold(int32 Amount);

    // 현재 보유 골드 반환
    UFUNCTION()
    int32 GetPlayerGold() const;

    // 광석 인벤토리에 추가
    UFUNCTION()
    void AddOreToInventory(FName OreID, int32 Amount);

    // 특정 광석 개수 반환
    UFUNCTION()
    int32 GetOreQuantity(FName OreID) const;

    // 현재 보유한 모든 광석 정보 반환
    UFUNCTION()
    const TArray<FOreInventoryItem>& GetAllOreData() const;

    // 광석 판매 (골드 추가 후 광석 차감)
    UFUNCTION()
    void SellOre(FName OreID, int32 SellPrice);
};
