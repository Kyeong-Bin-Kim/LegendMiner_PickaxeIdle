#include "PlayerSaveData.h"
#include "Kismet/GameplayStatics.h"
#include "Ore.h"

UPlayerSaveData::UPlayerSaveData()
{
    SaveSlotName = TEXT("PlayerSaveSlot");
    UserIndex = 0;
    InitializeSaveData();
}

// 저장 데이터 초기화
void UPlayerSaveData::InitializeSaveData()
{
    PickaxeLevel = 1;  // 기본 곡괭이 레벨
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

    // UI가 변경 사항을 감지할 수 있도록 이벤트 실행
    OnOreDataUpdated.Broadcast();
}

// 게임 로드 기능 (GameInstance에서 관리)
UPlayerSaveData* UPlayerSaveData::LoadGameData()
{
    if (UGameplayStatics::DoesSaveGameExist(TEXT("PlayerSaveSlot"), 0))
    {
        UPlayerSaveData* LoadedData = Cast<UPlayerSaveData>(UGameplayStatics::LoadGameFromSlot(TEXT("PlayerSaveSlot"), 0));
        if (LoadedData)
        {
            return LoadedData;
        }
    }

    return NewObject<UPlayerSaveData>();
}

// 곡괭이 레벨 설정
void UPlayerSaveData::SetPickaxeLevel(int32 NewLevel)
{
    PickaxeLevel = NewLevel;
    SaveGameData();

    OnOreDataUpdated.Broadcast();
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
            OnOreDataUpdated.Broadcast(); // UI 갱신 이벤트 발생
            return;
        }
    }

    OreInventory.Add(FOreInventoryItem(OreID, Amount));
    SaveGameData();
    OnOreDataUpdated.Broadcast(); // UI 갱신 이벤트 발생
}

void UPlayerSaveData::RemoveOreFromInventory(FName OreID, int32 Amount)
{
    for (int32 i = 0; i < OreInventory.Num(); i++)
    {
        if (OreInventory[i].OreID == OreID)
        {
            if (OreInventory[i].Quantity >= Amount)
            {
                OreInventory[i].Quantity -= Amount;
                if (OreInventory[i].Quantity <= 0)
                {
                    OreInventory.RemoveAt(i); // 개수가 0이면 제거
                }

                SaveGameData();
                OnOreDataUpdated.Broadcast();
                return;
            }
        }
    }
}

void UPlayerSaveData::SubtractGold(int32 Amount)
{
    if (PlayerGold >= Amount)
    {
        PlayerGold -= Amount;
        SaveGameData();
        OnOreDataUpdated.Broadcast();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("PlayerSaveData: Not enough gold!"));
    }
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

// 모든 광석 데이터 반환 (UI 갱신용)
const TArray<FOreInventoryItem>& UPlayerSaveData::GetAllOreData() const
{
    return OreInventory;
}

// 광석 판매 (골드 추가 후 광석 차감)
void UPlayerSaveData::SellOre(FName OreID, int32 SellPrice)
{
    bool bOreSold = false;
    int32 OreIndexToRemove = -1; // 삭제할 인덱스 추적

    for (int32 i = 0; i < OreInventory.Num(); i++)
    {
        if (OreInventory[i].OreID == OreID)
        {
            int32 Amount = OreInventory[i].Quantity; // 모든 개수 판매
            OreInventory[i].Quantity = 0;
            PlayerGold += SellPrice * Amount; // 골드 증가

            bOreSold = true;

            // 나중에 삭제할 인덱스 저장 (반복문 중 직접 삭제 X)
            OreIndexToRemove = i;
            break;
        }
    }

    // 반복문 종료 후 요소 제거 (배열 크기 변동 방지)
    if (OreIndexToRemove != -1)
    {
        OreInventory.RemoveAt(OreIndexToRemove);
    }

    if (bOreSold)
    {
        SaveGameData(); // 한 번만 저장

        // 판매 후 모든 AOre 인스턴스의 `CachedSaveData` 갱신
        TArray<AActor*> Ores;
        UGameplayStatics::GetAllActorsOfClass(GWorld, AOre::StaticClass(), Ores);
        for (AActor* OreActor : Ores)
        {
            AOre* Ore = Cast<AOre>(OreActor);
            if (Ore)
            {
                Ore->RefreshSaveData();
            }
        }

        OnOreDataUpdated.Broadcast(); // UI 갱신 이벤트 발생
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Ore not found in inventory!"));
    }
}


