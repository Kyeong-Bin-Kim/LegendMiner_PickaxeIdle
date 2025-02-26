#include "PlayerSaveData.h"
#include "Kismet/GameplayStatics.h"

UPlayerSaveData::UPlayerSaveData()
{
    SaveSlotName = TEXT("PlayerSaveSlot");
    UserIndex = 0;
    InitializeSaveData();
}

// 저장 데이터 초기화
void UPlayerSaveData::InitializeSaveData()
{
    PickaxeLevel = 14;  // 기본 곡괭이 레벨
    PlayerGold = 0;    // 초기 골드
    OreInventory.Empty(); // 광석 인벤토리 초기화
}

// 게임 저장 기능
void UPlayerSaveData::SaveGameData()
{
    if (UGameplayStatics::SaveGameToSlot(this, SaveSlotName, UserIndex))
    {
        UE_LOG(LogTemp, Log, TEXT("PlayerSaveData: Save successful!"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerSaveData: Save failed!"));
    }
}

// 게임 로드 기능
UPlayerSaveData* UPlayerSaveData::LoadGameData()
{
    if (UGameplayStatics::DoesSaveGameExist(TEXT("PlayerSaveSlot"), 0))
    {
        UPlayerSaveData* LoadedData = Cast<UPlayerSaveData>(UGameplayStatics::LoadGameFromSlot(TEXT("PlayerSaveSlot"), 0));
        if (LoadedData)
        {
            UE_LOG(LogTemp, Log, TEXT("PlayerSaveData: Load successful!"));
            return LoadedData;
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("PlayerSaveData: No save found, creating new save."));
    return NewObject<UPlayerSaveData>();
}

// 곡괭이 레벨 설정
void UPlayerSaveData::SetPickaxeLevel(int32 NewLevel)
{
    PickaxeLevel = NewLevel;
    SaveGameData();
}

// 곡괭이 레벨 반환
int32 UPlayerSaveData::GetPickaxeLevel() const
{
    return PickaxeLevel;
}

// 골드 추가
void UPlayerSaveData::AddGold(int32 Amount)
{
    PlayerGold += Amount;
    SaveGameData();
}

// 현재 보유 골드 반환
int32 UPlayerSaveData::GetPlayerGold() const
{
    return PlayerGold;
}

// 광석을 인벤토리에 추가
void UPlayerSaveData::AddOreToInventory(FName OreID, int32 Amount)
{
    for (FOreInventoryItem& Item : OreInventory)
    {
        if (Item.OreID == OreID)
        {
            Item.Quantity += Amount;
            SaveGameData();
            return;
        }
    }

    // 기존에 없으면 새로 추가
    OreInventory.Add(FOreInventoryItem(OreID, Amount));
    SaveGameData();
}

// 특정 광석 개수 반환
int32 UPlayerSaveData::GetOreQuantity(FName OreID) const
{
    for (const FOreInventoryItem& Item : OreInventory)
    {
        if (Item.OreID == OreID)
        {
            return Item.Quantity;
        }
    }
    return 0;
}

// 광석 판매 (골드 추가 후 광석 차감)
void UPlayerSaveData::SellOre(FName OreID, int32 Amount, int32 SellPrice)
{
    for (int32 i = 0; i < OreInventory.Num(); i++)
    {
        if (OreInventory[i].OreID == OreID)
        {
            if (OreInventory[i].Quantity >= Amount)
            {
                OreInventory[i].Quantity -= Amount;
                PlayerGold += SellPrice * Amount;
                UE_LOG(LogTemp, Warning, TEXT("Sold %d of %s. New Gold: %d"), Amount, *OreID.ToString(), PlayerGold);

                // 개수가 0이면 배열에서 제거
                if (OreInventory[i].Quantity <= 0)
                {
                    OreInventory.RemoveAt(i);
                }
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Not enough ore to sell!"));
            }
            return;
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("Ore not found in inventory!"));
}

