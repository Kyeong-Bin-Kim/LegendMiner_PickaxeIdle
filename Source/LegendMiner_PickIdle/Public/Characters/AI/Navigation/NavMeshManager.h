#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NavMeshManager.generated.h"

UCLASS()
class LEGENDMINER_PICKIDLE_API ANavMeshManager : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    ANavMeshManager();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Function to generate NavMesh
    UFUNCTION(BlueprintCallable, Category = "NavMesh")
    void GenerateNavMesh();

private:
    void CalculateBounds();
    void CreateNavMeshBoundsVolume();

    FVector MinBounds;
    FVector MaxBounds;
};