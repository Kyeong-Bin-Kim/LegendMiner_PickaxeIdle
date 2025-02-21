#include "PickaxeComponent.h"

UPickaxeComponent::UPickaxeComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UPickaxeComponent::BeginPlay()
{
    Super::BeginPlay();

    AActor* Owner = GetOwner();
    if (Owner)
    {
        StaticMeshComponent = Owner->FindComponentByClass<UStaticMeshComponent>();

        if (!StaticMeshComponent)
        {
            UE_LOG(LogTemp, Error, TEXT("UPickaxeComponent: Failed to find StaticMeshComponent in owner actor!"));
        }
    }
}

void UPickaxeComponent::UpdatePickaxeData()
{
    if (!PickaxeDataTable)
    {
        UE_LOG(LogTemp, Error, TEXT("PickaxeDataTable is NULL! Please assign a valid DataTable."));
        return;
    }

    FName RowName = FName(*FString::FromInt(PickaxeLevel));
    FPickaxeData* Data = PickaxeDataTable->FindRow<FPickaxeData>(RowName, TEXT(""));

    if (Data)
    {
        CurrentPickaxeData = *Data;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Pickaxe Level %d not found in DataTable!"), PickaxeLevel);
    }
}

void UPickaxeComponent::SetPickaxeMesh()
{
    // StaticMeshComponent가 nullptr이면 크래시 방지
    if (!StaticMeshComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("SetPickaxeMesh failed: StaticMeshComponent is NULL!"));
        return;
    }

    // CurrentPickaxeData.Mesh가 nullptr이면 크래시 방지
    if (CurrentPickaxeData.Mesh)
    {
        StaticMeshComponent->SetStaticMesh(CurrentPickaxeData.Mesh);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("SetPickaxeMesh: Mesh is NULL!"));
    }

    if (CurrentPickaxeData.Material)
    {
        StaticMeshComponent->SetMaterial(0, CurrentPickaxeData.Material);
    }

    if (EffectComponent)
    {
        EffectComponent->Deactivate();

        if (CurrentPickaxeData.Effect)
        {
            EffectComponent->SetAsset(CurrentPickaxeData.Effect);
            EffectComponent->Activate();
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("SetPickaxeMesh: Effect is NULL!"));
        }
    }
    else
    {
        if (CurrentPickaxeData.Effect)
        {
            EffectComponent->SetAsset(CurrentPickaxeData.Effect);
            EffectComponent->Activate();
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("SetPickaxeMesh: Effect is NULL!"));
        }
    }
}
