#include "Ore.h"
#include "OreSpawner.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"

AOre::AOre()
{
    PrimaryActorTick.bCanEverTick = false;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

    // 메쉬를 RootComponent에 부착
    OreMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OreMesh"));
    OreMesh->SetupAttachment(RootComponent);

    // 원형 콜리전 설정
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
    OreHealth = 300.0f;

    UE_LOG(LogTemp, Warning, TEXT("AOre: Initialized with Level %d, Health: %f"), OreLevel, OreHealth);

    if (Tags.Contains(FName("Ore")))
    {
        UE_LOG(LogTemp, Warning, TEXT("AOre: Ore tag successfully added!"));
    }

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
        return;
    }

    if (Data->Mesh)
    {
        OreMesh->SetStaticMesh(Data->Mesh);
        UE_LOG(LogTemp, Warning, TEXT("AOre: Mesh Set for Level %d"), OreLevel);

        // 메쉬 크기 가져와 원형 콜리전 크기 조정
        FVector MeshBounds = OreMesh->Bounds.BoxExtent;
        float SphereRadius = FMath::Max(MeshBounds.X, MeshBounds.Y) + 100.0f;
        OreTrigger->SetSphereRadius(SphereRadius);
        UE_LOG(LogTemp, Warning, TEXT("AOre: OreTrigger Set to Radius %s"), *FString::SanitizeFloat(SphereRadius));


    }

    if (Data->Material)
    {
        OreMesh->SetMaterial(0, Data->Material);
        UE_LOG(LogTemp, Warning, TEXT("AOre: Material Set for Level %d"), OreLevel);
    }
}

void AOre::MineOre()
{
    OreHealth -= 1.0f;
    UE_LOG(LogTemp, Warning, TEXT("AOre: Mined! Remaining Health: %f"), OreHealth);

    if (OreHealth <= 0.0f)
    {
        DestroyOre();
    }
}

void AOre::DestroyOre()
{
    if (SpawnerRef)
    {
        SpawnerRef->ReplaceOre(this);
    }

    Destroy();
}
