#include "AI/Navigation/NavMeshManager.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "NavMesh/NavMeshBoundsVolume.h"

// Sets default values
ANavMeshManager::ANavMeshManager()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ANavMeshManager::BeginPlay()
{
    Super::BeginPlay();

    CalculateBounds();
    CreateNavMeshBoundsVolume();
    GenerateNavMesh();
}

// Called every frame
void ANavMeshManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ANavMeshManager::CalculateBounds()
{
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Ground"), FoundActors);

    if (FoundActors.Num() > 0)
    {
        MinBounds = FoundActors[0]->GetActorLocation();
        MaxBounds = FoundActors[0]->GetActorLocation();

        for (AActor* Actor : FoundActors)
        {
            FVector ActorLocation = Actor->GetActorLocation();
            FVector ActorBounds = Actor->GetComponentsBoundingBox().GetExtent();

            MinBounds = MinBounds.ComponentMin(ActorLocation - ActorBounds);
            MaxBounds = MaxBounds.ComponentMax(ActorLocation + ActorBounds);
        }
    }
}

void ANavMeshManager::CreateNavMeshBoundsVolume()
{
    FBox Bounds(MinBounds, MaxBounds);
    FVector Center = Bounds.GetCenter();
    FVector Extent = Bounds.GetExtent();

    ANavMeshBoundsVolume* NavMeshBoundsVolume = GetWorld()->SpawnActor<ANavMeshBoundsVolume>(Center, FRotator::ZeroRotator);
    if (NavMeshBoundsVolume)
    {
        NavMeshBoundsVolume->GetRootComponent()->SetWorldScale3D(Extent * 2.0f / 100.0f); // Convert from cm to m
        UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
        if (NavSys)
        {
            NavSys->OnNavigationBoundsUpdated(NavMeshBoundsVolume);
        }
    }
}

void ANavMeshManager::GenerateNavMesh()
{
    UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    if (NavSys)
    {
        NavSys->Build();
    }
}