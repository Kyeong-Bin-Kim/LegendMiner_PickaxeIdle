#include "LegendMinerGameMode.h"
#include "AI/PlayerAIController.h"
#include "PlayerCharacter.h"
#include "AI/Navigation/NavMeshManager.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

ALegendMinerGameMode::ALegendMinerGameMode()
{

}

void ALegendMinerGameMode::BeginPlay()
{
    Super::BeginPlay();

    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANavMeshManager::StaticClass(), FoundActors);

    ANavMeshManager* NavMeshManager = nullptr;

    if (FoundActors.Num() == 0)
    {
        NavMeshManager = GetWorld()->SpawnActor<ANavMeshManager>();
        UE_LOG(LogTemp, Warning, TEXT("NavMeshManager 자동 생성됨!"));
    }
    else
    {
        NavMeshManager = Cast<ANavMeshManager>(FoundActors[0]);
    }

    if (NavMeshManager)
    {
        NavMeshManager->GenerateNavMesh();
	}
    else
    {
        UE_LOG(LogTemp, Error, TEXT("NavMeshManager를 찾을 수 없습니다!"));
    }
}