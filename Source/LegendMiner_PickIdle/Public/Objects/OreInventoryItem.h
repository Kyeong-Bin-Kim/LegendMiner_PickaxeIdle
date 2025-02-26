#pragma once

#include "CoreMinimal.h"
#include "OreInventoryItem.generated.h"

USTRUCT(BlueprintType)
struct FOreInventoryItem
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ore")
    FName OreID; // 광석 ID

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ore")
    int32 Quantity; // 보유 개수

    FOreInventoryItem()
        : OreID(TEXT("None")), Quantity(0) {}

    FOreInventoryItem(FName InOreID, int32 InQuantity)
        : OreID(InOreID), Quantity(InQuantity) {}

    bool operator==(const FOreInventoryItem& Other) const
    {
        return OreID == Other.OreID;
    }
};