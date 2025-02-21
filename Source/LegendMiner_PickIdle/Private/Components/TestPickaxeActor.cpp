#include "TestPickaxeActor.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"

ATestPickaxeActor::ATestPickaxeActor()
{
    PrimaryActorTick.bCanEverTick = false;

    PickaxeMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickaxeMeshComponent"));
    RootComponent = PickaxeMeshComponent;

    PickaxeComponent = CreateDefaultSubobject<UPickaxeComponent>(TEXT("PickaxeComponent"));
}

void ATestPickaxeActor::BeginPlay()
{
    Super::BeginPlay();

    if (PickaxeComponent)
    {
        PickaxeComponent->UpdatePickaxeData();
        PickaxeComponent->SetPickaxeMesh();
    }
}