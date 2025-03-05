#include "Ore.h"
#include "OreSpawner.h"
#include "PlayerCharacter.h"
#include "PickaxeComponent.h"
#include "PlayerSaveData.h"
#include "PlayerInventoryWidget.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/Engine.h"
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
        UE_LOG(LogTemp, Error, TEXT("AOre: Failed to cache PlayerSaveData!"));
    }

    if (!PlayerRef)
    {
        UE_LOG(LogTemp, Error, TEXT("AOre: Failed to cache PlayerCharacter!"));
    }

}

void AOre::InitializeOre(int32 InOreLevel, AOreSpawner* InSpawner)
{
    OreLevel = InOreLevel;
    SpawnerRef = InSpawner;

    UE_LOG(LogTemp, Warning, TEXT("AOre: Initialized with Level %d"), OreLevel);

    Tags.Add(FName("Ore"));

    UpdateOreAppearance();
}

void AOre::UpdateOreAppearance()
{
    if (!SpawnerRef || !SpawnerRef->OreDataTable)
    {
        UE_LOG(LogTemp, Error, TEXT("AOre: SpawnerRef or OreDataTable is NULL"));
        return;
    }

    FName RowName = FName(*FString::FromInt(OreLevel));
    FOreData* Data = SpawnerRef->OreDataTable->FindRow<FOreData>(RowName, TEXT(""));

    if (!Data)
    {
        UE_LOG(LogTemp, Error, TEXT("AOre: No data found for OreLevel: %d"), OreLevel);

        // 데이터가 없으면 가능한 최대 OreLevel 사용
        OreLevel = GetMaxOreLevel();
        RowName = FName(*FString::FromInt(OreLevel));
        Data = SpawnerRef->OreDataTable->FindRow<FOreData>(RowName, TEXT(""));

        if (!Data)
        {
            UE_LOG(LogTemp, Error, TEXT("AOre: No valid OreLevel found even after searching for the max level!"));
            return;
        }

        UE_LOG(LogTemp, Warning, TEXT("AOre: OreLevel updated to highest available level: %d"), OreLevel);
    }

    if (Data->Mesh)
    {
        OreMesh->SetStaticMesh(Data->Mesh);
        FVector MeshBounds = OreMesh->Bounds.BoxExtent;
        float SphereRadius = FMath::Max(MeshBounds.X, MeshBounds.Y) + 100.0f;
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
    if (!Player || !SpawnerRef || !SpawnerRef->OreDataTable) return;

    PlayerRef = Player;

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

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow,
            FString::Printf(TEXT("채굴 보너스: %.2f 초"), PickaxeBonus));
        GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow,
            FString::Printf(TEXT("채굴 속도: %.2f 초"), CalculatedMiningTime));
    }

    GetWorldTimerManager().SetTimer(MiningTimerHandle, this, &AOre::MineOre, MiningTime * 1.166667, true);

    UE_LOG(LogTemp, Warning, TEXT("AOre: Started mining with interval: %.2f"), CalculatedMiningTime);
}

void AOre::MineOre()
{
    // UI 업데이트
    if (PlayerRef)
    {
        if (CachedSaveData)
        {
            FName MinedOreID = FName(*FString::FromInt(OreLevel));

            CachedSaveData->AddOreToInventory(MinedOreID, 1);

            int32 GetQuantity = CachedSaveData->GetOreQuantity(MinedOreID);

            PlayerRef->CachedInventoryWidget->UpdateSingleOreQuantity(MinedOreID, GetQuantity);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("AOre: CachedInventoryWidget is NULL!"));
        }
    }
}

void AOre::StopMining()
{
    GetWorldTimerManager().ClearTimer(MiningTimerHandle);
}

void AOre::RefreshSaveData()
{
    CachedSaveData = UPlayerSaveData::LoadGameData();
    if (CachedSaveData)
    {
        UE_LOG(LogTemp, Warning, TEXT("AOre: Cached SaveData refreshed successfully!"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("AOre: Failed to refresh Cached SaveData!"));
    }
}
