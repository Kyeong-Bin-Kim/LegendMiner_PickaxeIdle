#include "OreSpawner.h"
#include "Ore.h"
#include "PlayerSaveData.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "Engine/DataTable.h"
#include "NavigationSystem.h"

AOreSpawner::AOreSpawner()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AOreSpawner::BeginPlay()
{
    Super::BeginPlay();

    // OreDataTable이 올바르게 설정되었는지 확인
    if (!OreDataTable)
    {
        UE_LOG(LogTemp, Error, TEXT("AOreSpawner: OreDataTable is NULL! Make sure to set it in BP_OreSpawner."));
        return;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("AOreSpawner: Loaded OreDataTable successfully!"));
    }

    // 박스 콜리전 찾아서 등록
    FindSpawnBoxes();

    if (SpawnBoxes.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("AOreSpawner: SpawnBoxes is EMPTY! Make sure to add Box Collisions in BP_OreSpawner."));
        return;
    }

    SpawnInitialOres();

    UpdateNavMesh();
}

// NavMesh 갱신
void AOreSpawner::UpdateNavMesh()
{
    UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    if (NavSystem)
    {
        NavSystem->Build();
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("AOreSpawner: Failed to get NavigationSystem!"));
    }
}

// 박스 콜리전 찾아서 등록
void AOreSpawner::FindSpawnBoxes()
{
    TArray<UActorComponent*> Components;
    GetComponents(Components); // AOreSpawner의 모든 컴포넌트 가져옴

    for (UActorComponent* Component : Components)
    {
        UBoxComponent* Box = Cast<UBoxComponent>(Component);
        if (Box)
        {
            SpawnBoxes.Add(Box);
        }
    }

    if (SpawnBoxes.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("AOreSpawner: No SpawnBoxes found! Make sure BP_OreSpawner has BoxComponents."));
    }
}

// 플레이어의 곡괭이 레벨 가져오기
int32 AOreSpawner::GetPlayerPickaxeLevel()
{
    PlayerSaveData = Cast<UPlayerSaveData>(UGameplayStatics::LoadGameFromSlot(TEXT("PlayerSaveSlot"), 0));

    if (!PlayerSaveData)
    {
        PlayerSaveData = Cast<UPlayerSaveData>(UGameplayStatics::CreateSaveGameObject(UPlayerSaveData::StaticClass()));
        UE_LOG(LogTemp, Warning, TEXT("No existing save found. Created new PlayerSaveData."));
    }

    if (PlayerSaveData)
    {
        return PlayerSaveData->PickaxeLevel;
    }

    return 1;
}

int32 AOreSpawner::GetMaxOreLevelFromDataTable() const
{
    if (!OreDataTable) return 1;

    TArray<FName> RowNames = OreDataTable->GetRowNames();
    int32 MaxOreLevel = 1;

    for (const FName& RowName : RowNames)
    {
        int32 Level = FCString::Atoi(*RowName.ToString());
        MaxOreLevel = FMath::Max(MaxOreLevel, Level);
    }

    return MaxOreLevel;
}

// 플레이어 곡괭이 레벨 이하의 랜덤 광석 레벨 반환
int32 AOreSpawner::GetRandomOreLevel(int32 PlayerPickaxeLevel)
{
    const int32 MinAllowedDifference = 3;

    int32 MaxOreLevelFromData = GetMaxOreLevelFromDataTable();
    int32 OreMax = FMath::Clamp(PlayerPickaxeLevel, 1, MaxOreLevelFromData);

    int32 RawMin = OreMax - MinAllowedDifference;
    int32 OreMin = FMath::Clamp(RawMin, 1, OreMax);

    return FMath::RandRange(OreMin, OreMax);
}

// 콜리전 내부에서 랜덤한 위치 반환
FVector AOreSpawner::GetRandomSpawnLocation()
{
    FVector SpawnLocation = FVector::ZeroVector;
    bool bLocationValid = false;
    int32 MaxAttempts = 10;
    float MinSpacing = 350.0f; // 최소 거리 유지

    for (int32 Attempt = 0; Attempt < MaxAttempts; Attempt++)
    {
        int32 RandomIndex = FMath::RandRange(0, SpawnBoxes.Num() - 1);
        UBoxComponent* SelectedBox = SpawnBoxes[RandomIndex];

        if (!SelectedBox) continue;

        FVector Origin = SelectedBox->GetComponentLocation();
        FVector Extent = SelectedBox->GetScaledBoxExtent();

        FVector CandidateLocation = FVector(
            FMath::RandRange(Origin.X - Extent.X, Origin.X + Extent.X),
            FMath::RandRange(Origin.Y - Extent.Y, Origin.Y + Extent.Y),
            Origin.Z + Extent.Z * 0.5f
        );

        bool bIsFarEnough = true;
        float ClosestDistance = FLT_MAX;

        for (AOre* ExistingOre : ActiveOres)
        {
            if (ExistingOre)
            {
                float Distance = FVector::Dist(ExistingOre->GetActorLocation(), CandidateLocation);
                ClosestDistance = FMath::Min(ClosestDistance, Distance);

                if (Distance < MinSpacing)
                {
                    bIsFarEnough = false;
                    break;
                }
            }
        }

        if (bIsFarEnough)
        {
            SpawnLocation = CandidateLocation;
            bLocationValid = true;
            break;
        }
    }

    if (!bLocationValid)
    {
        UE_LOG(LogTemp, Warning, TEXT("AOreSpawner: Could not find valid spawn location after %d attempts. Skipping spawn."), MaxAttempts);
        return FVector::ZeroVector; // 유효한 위치를 찾지 못하면 ZeroVector 반환
    }

    return SpawnLocation;
}

// 광석 초기 생성
void AOreSpawner::SpawnInitialOres()
{
    if (!OreDataTable || SpawnBoxes.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("AOreSpawner: OreDataTable or SpawnBoxes is NULL!"));
        return;
    }

    int32 PlayerPickaxeLevel = GetPlayerPickaxeLevel();
    int32 MaxOreLevel = GetMaxOreLevel(PlayerPickaxeLevel);

    for (int32 i = 0; i < MaxOreCount; i++)
    {
        FVector SpawnLocation = GetRandomSpawnLocation();

        // 위치 찾기 실패 시 스폰하지 않음 (0,0,0 방지)
        if (SpawnLocation.IsZero())
        {
            UE_LOG(LogTemp, Warning, TEXT("AOreSpawner: Skipping ore spawn due to invalid location."));
            continue;
        }

        int32 RandomOreLevel = GetRandomOreLevel(PlayerPickaxeLevel);

        // 유효한 OreLevel이 없는 경우 → MaxOreLevel 적용
        if (RandomOreLevel <= 0)
        {
            RandomOreLevel = MaxOreLevel;
        }

        AOre* NewOre = GetWorld()->SpawnActor<AOre>(AOre::StaticClass(), SpawnLocation, FRotator::ZeroRotator);
        if (NewOre)
        {
            NewOre->InitializeOre(RandomOreLevel, this);
            NewOre->Tags.Add(FName("Ore")); // "Ore" 태그 추가
            ActiveOres.Add(NewOre);
        }
    }
}


// 곡괭이 레벨을 고려한 최대 OreLevel 계산
int32 AOreSpawner::GetMaxOreLevel(int32 PlayerPickaxeLevel)
{
    if (!OreDataTable) return 1;

    TArray<FName> RowNames = OreDataTable->GetRowNames();
    int32 MaxOreLevel = 1; // 최소 기본값 설정

    for (FName RowName : RowNames)
    {
        FOreData* OreData = OreDataTable->FindRow<FOreData>(RowName, TEXT(""));
        if (OreData)
        {
            MaxOreLevel = FMath::Max(MaxOreLevel, OreData->Strength);
        }
    }

    // 곡괭이 레벨이 최대 레벨일 때만 MaxOreLevel을 (곡괭이 최대 레벨 -1) 로 설정
    if (PlayerPickaxeLevel >= MaxOreLevel)
    {
        MaxOreLevel = FMath::Max(1, PlayerPickaxeLevel - 1);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("AOreSpawner: Using normal MaxOreLevel: %d"), MaxOreLevel);
    }

    return MaxOreLevel;
}

void AOreSpawner::RespawnAllOres()
{
    if (!GetWorld()) return;

    // 기존 광석 제거
    for (AOre* Ore : ActiveOres)
    {
        if (Ore)
        {
            Ore->Destroy();
        }
    }
    ActiveOres.Empty(); // 리스트 초기화

    // 새로운 광석 생성
    SpawnInitialOres();

    // NavMesh 업데이트 (광석이 새롭게 배치될 경우 AI 이동 경로 수정 필요)
    UpdateNavMesh();
}
