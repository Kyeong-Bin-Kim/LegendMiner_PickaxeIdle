#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OreData.generated.h"

USTRUCT(Atomic, BlueprintType)
struct FOreData : public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Level;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName ResourceID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText ResourceName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MiningTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Quantity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Rarity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Strength;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UStaticMesh> Mesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UMaterialInterface> Material;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UTexture2D> Icon;
};