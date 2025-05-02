#include "Ore.h"
#include "OreSpawner.h"
#include "PlayerCharacter.h"
#include "PickaxeComponent.h"
#include "PlayerSaveData.h"
#include "PlayerInventoryWidget.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

AOre::AOre()
{
    PrimaryActorTick.bCanEverTick = false;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

    OreMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OreMesh"));
    OreMesh->SetupAttachment(RootComponent);

    OreTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("OreTrigger"));
    OreTrigger->SetupAttachment(RootComponent);
    OreTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    OreTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
    OreTrigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AOre::BeginPlay()
{
    Super::BeginPlay();

    // 플레이어 저장 데이터를 한 번만 캐싱
    CachedSaveData = UPlayerSaveData::LoadGameData();

    // 플레이어 캐싱
    PlayerRef = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

    if (!CachedSaveData)
    {
        UE_LOG(LogTemp, Error, TEXT("AOre: 플레이어 저장 데이터를 캐싱하는 데 실패했습니다!"));
    }

    if (!PlayerRef)
    {
        UE_LOG(LogTemp, Error, TEXT("AOre: 플레이어 캐릭터 캐싱에 실패했습니다!"));
    }

}

void AOre::InitializeOre(int32 InOreLevel, AOreSpawner* InSpawner)
{
    OreLevel = InOreLevel;
    SpawnerRef = InSpawner;

    Tags.Add(FName("Ore"));

    UpdateOreAppearance();
}

void AOre::UpdateOreAppearance()
{
    if (!SpawnerRef || !SpawnerRef->OreDataTable)
    {
        UE_LOG(LogTemp, Error, TEXT("AOre: 스포너 또는 광석 데이터 테이블이 없습니다."));
        return;
    }

    FName RowName = FName(*FString::FromInt(OreLevel));
    FOreData* Data = SpawnerRef->OreDataTable->FindRow<FOreData>(RowName, TEXT(""));

    if (!Data)
    {
        // 데이터가 없으면 가능한 최대 OreLevel 사용
        OreLevel = GetMaxOreLevel();
        RowName = FName(*FString::FromInt(OreLevel));
        Data = SpawnerRef->OreDataTable->FindRow<FOreData>(RowName, TEXT(""));

        if (!Data)
        {
            UE_LOG(LogTemp, Error, TEXT("AOre: 최대 레벨을 검색했지만 유효한 OreLevel을 찾을 수 없습니다!"));
            return;
        }
    }

    if (Data->Mesh)
    {
        OreMesh->SetStaticMesh(Data->Mesh);
        FVector MeshBounds = OreMesh->Bounds.BoxExtent;
        float SphereRadius = FMath::Max(MeshBounds.X, MeshBounds.Y) + 150.0f;
        OreTrigger->SetSphereRadius(SphereRadius);
    }

    if (Data->Material)
    {
        OreMesh->SetMaterial(0, Data->Material);
    }
}

int32 AOre::GetMaxOreLevel() const
{
    if (!SpawnerRef || !SpawnerRef->OreDataTable) return 1;

    int32 MaxLevel = 1;
    TArray<FName> RowNames = SpawnerRef->OreDataTable->GetRowNames();

    for (FName RowName : RowNames)
    {
        FOreData* Data = SpawnerRef->OreDataTable->FindRow<FOreData>(RowName, TEXT(""));
        if (Data)
        {
            int32 Level = FCString::Atoi(*RowName.ToString());
            MaxLevel = FMath::Max(MaxLevel, Level);
        }
    }
    return MaxLevel;
}

void AOre::StartMining(APlayerCharacter* Player)
{
    if (!Player || !SpawnerRef || !SpawnerRef->OreDataTable)
        return;

    PlayerRef = Player;

    CachedSaveData = UPlayerSaveData::LoadGameData();

    FName RowName = FName(*FString::FromInt(OreLevel));
    FOreData* OreData = SpawnerRef->OreDataTable->FindRow<FOreData>(RowName, TEXT(""));
    if (!OreData) return;

    float PickaxeBonus = Player->GetMiningSpeedBonus();

    float CalculatedMiningTime = 0.f;

    if (PickaxeBonus <= 0.f)
    {
        CalculatedMiningTime = OreData->MiningTime;
    }
    else
    {
        CalculatedMiningTime = OreData->MiningTime / PickaxeBonus;
    }

    // 계산된 채굴 시간을 MiningTime에 할당
    MiningTime = CalculatedMiningTime;
}

void AOre::MineOre()
{
    if (!PlayerRef)
    {
        UE_LOG(LogTemp, Warning, TEXT("AOre: PlayerRef가 비어있습니다."));
        return;
    }

    if (!CachedSaveData)
    {
        UE_LOG(LogTemp, Warning, TEXT("AOre: 저장 데이터가 NULL입니다. 새로 불러옵니다."));
        CachedSaveData = UPlayerSaveData::LoadGameData();
    }

    if (!CachedSaveData)
    {
        UE_LOG(LogTemp, Error, TEXT("AOre: 저장 데이터를 불러오는 데 실패했습니다!"));
        return;
    }

    FName MinedOreID = FName(*FString::FromInt(OreLevel));

    // 최신 데이터에 광석 추가
    CachedSaveData->AddOreToInventory(MinedOreID, 1);
    CachedSaveData->SaveGameData(); // 즉시 저장

    // UI 갱신
    if (PlayerRef->CachedInventoryWidget)
    {
        int32 GetQuantity = CachedSaveData->GetOreQuantity(MinedOreID);
        PlayerRef->CachedInventoryWidget->UpdateSingleOreQuantity(MinedOreID, GetQuantity);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("AOre: 인벤토리 UI가 존재하지 않습니다!"));
    }
}

void AOre::RefreshSaveData()
{
    CachedSaveData = UPlayerSaveData::LoadGameData();
    if (CachedSaveData)
    {
        UE_LOG(LogTemp, Warning, TEXT("AOre: 저장 데이터를 성공적으로 갱신했습니다."));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("AOre: 저장 데이터 갱신에 실패했습니다."));
    }
}
