#include "Ore.h"
#include "OreSpawner.h"
#include "PlayerCharacter.h"
#include "PickaxeComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/Engine.h"
#include "TimerManager.h"

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
}

void AOre::InitializeOre(int32 InOreLevel, AOreSpawner* InSpawner)
{
    OreLevel = InOreLevel;
    SpawnerRef = InSpawner;
    OreHealth = 10.0f;

    UE_LOG(LogTemp, Warning, TEXT("AOre: Initialized with Level %d, Health: %f"), OreLevel, OreHealth);

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

        // 가장 높은 OreLevel 값 찾기
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

    FName RowName = FName(*FString::FromInt(OreLevel));
    FOreData* OreData = SpawnerRef->OreDataTable->FindRow<FOreData>(RowName, TEXT(""));
    if (!OreData) return;

    float PickaxeBonus = Player->GetMiningSpeedBonus();
    float MiningTime = OreData->MiningTime - (OreData->MiningTime * PickaxeBonus);
    MiningTime = FMath::Max(MiningTime, 0.1f);

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            -1, 3.0f, FColor::Yellow,
            FString::Printf(TEXT("채굴 보너스: %.2f 초"), PickaxeBonus)
        );

        GEngine->AddOnScreenDebugMessage(
            -1, 3.0f, FColor::Yellow,
            FString::Printf(TEXT("채굴 속도: %.2f 초"), MiningTime)
        );
    }

    // 기존 타이머를 클리어하고 새롭게 설정
    GetWorldTimerManager().ClearTimer(MiningTimerHandle);
    GetWorldTimerManager().SetTimer(MiningTimerHandle, this, &AOre::MineOre, MiningTime, true);

    UE_LOG(LogTemp, Warning, TEXT("AOre: Started mining with interval: %.2f"), MiningTime);
}

void AOre::MineOre()
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, FString::Printf(TEXT("Start 광석 체력: %f"), OreHealth));
    }

    OreHealth -= 1; // 체력 감소를 먼저 수행

    if (OreHealth <= 0)
    {
        StopMining();
        DestroyOre();
        return;
    }

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, FString::Printf(TEXT("End 광석 체력: %f"), OreHealth));
    }
}

void AOre::StopMining()
{
    GetWorldTimerManager().ClearTimer(MiningTimerHandle);
    UE_LOG(LogTemp, Warning, TEXT("AOre: Mining stopped."));
}

void AOre::DestroyOre()
{
    if (SpawnerRef)
    {
        SpawnerRef->ReplaceOre(this);
    }

    Destroy();
}
