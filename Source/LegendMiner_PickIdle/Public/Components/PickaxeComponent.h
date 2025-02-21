#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PickaxeData.h"
#include "NiagaraComponent.h"
#include "PickaxeComponent.generated.h"


UCLASS()
class LEGENDMINER_PICKIDLE_API UPickaxeComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UPickaxeComponent();

protected:
    virtual void BeginPlay() override;

public:
    UFUNCTION(BlueprintCallable)
    void UpdatePickaxeData();

    UFUNCTION(BlueprintCallable)
    void SetPickaxeMesh();

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickaxe")
    int32 PickaxeLevel = 1;

private:
    UPROPERTY(EditAnywhere, Category = "Components")
    TObjectPtr<UDataTable> PickaxeDataTable;

    UPROPERTY(VisibleDefaultsOnly)
    TObjectPtr<UStaticMeshComponent> StaticMeshComponent;

    UPROPERTY(VisibleDefaultsOnly)
    TObjectPtr<UNiagaraComponent> EffectComponent;

    FPickaxeData CurrentPickaxeData;
};
