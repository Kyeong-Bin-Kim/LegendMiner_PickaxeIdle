#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"

UCLASS(meta = (BlueprintSpawnableComponent))
class LEGENDMINER_PICKIDLE_API UInteractionComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UInteractionComponent();

protected:
    virtual void BeginPlay() override;

public:
    // 상호작용 기능 (플레이어가 상호작용 시 호출됨)
    UFUNCTION(BlueprintCallable, Category = "Interaction")
    bool CanPlayerMine(int32 PlayerPickaxeLevel, int32 RequiredPickaxeLevel);
};
