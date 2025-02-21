#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "PickaxeData.generated.h"

USTRUCT(Atomic, BlueprintType)
struct FPickaxeData : public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Level;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Strength;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MiningSpeedBonus;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 UpgradeCostOre;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 UpgradeCostGold;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UStaticMesh> Mesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UMaterialInterface> Material;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UNiagaraSystem> Effect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UTexture2D> Icon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Name;
};