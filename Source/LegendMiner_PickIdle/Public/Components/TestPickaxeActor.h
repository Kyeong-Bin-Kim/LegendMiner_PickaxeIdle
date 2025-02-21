#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickaxeComponent.h"
#include "TestPickaxeActor.generated.h"

UCLASS()
class LEGENDMINER_PICKIDLE_API ATestPickaxeActor : public AActor
{
    GENERATED_BODY()

public:
    ATestPickaxeActor();

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UStaticMeshComponent> PickaxeMeshComponent;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UPickaxeComponent> PickaxeComponent;
};

