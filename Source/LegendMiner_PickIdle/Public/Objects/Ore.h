#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OreData.h"
#include "Ore.generated.h"

class AOreSpawner;
class UStaticMeshComponent;
class USphereComponent;

UCLASS()
class LEGENDMINER_PICKIDLE_API AOre : public AActor
{
    GENERATED_BODY()

public:
    AOre();

protected:
    virtual void BeginPlay() override;

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ore")
    UStaticMeshComponent* OreMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ore")
    USphereComponent* OreTrigger;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ore")
    int32 OreLevel;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ore")
    float OreHealth;

    AOreSpawner* SpawnerRef;

    UFUNCTION()
    void InitializeOre(int32 InOreLevel, AOreSpawner* InSpawner);

    void UpdateOreAppearance();
    void MineOre();
    void DestroyOre();
};
