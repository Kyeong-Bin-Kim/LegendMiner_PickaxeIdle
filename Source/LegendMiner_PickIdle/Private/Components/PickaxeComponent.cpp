#include "PickaxeComponent.h"
#include "PlayerSaveData.h"
#include "GameFramework/Character.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"

UPickaxeComponent::UPickaxeComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    // 곡괭이 메쉬 컴포넌트 생성
    StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickaxeMesh"));

    // 곡괭이 이펙트 컴포넌트 생성
    EffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("PickaxeEffect"));

    MiningSpeedBonus = -1.0f;
}

void UPickaxeComponent::BeginPlay()
{
    Super::BeginPlay();

    // 저장된 데이터에서 곡괭이 레벨 불러오기
    LoadPickaxeLevelFromSave();

    // 곡괭이 데이터 업데이트
    UpdatePickaxeData();

    // 곡괭이 외형 설정
    SetPickaxeMesh();

    // 캐릭터 손에 부착
    AttachPickaxeToHand();
}

// 저장된 데이터에서 곡괭이 레벨을 불러오기
void UPickaxeComponent::LoadPickaxeLevelFromSave()
{
    PlayerSaveData = Cast<UPlayerSaveData>(UGameplayStatics::LoadGameFromSlot(TEXT("PlayerSaveSlot"), 0));

    if (!PlayerSaveData)
    {
        PlayerSaveData = Cast<UPlayerSaveData>(UGameplayStatics::CreateSaveGameObject(UPlayerSaveData::StaticClass()));
        UE_LOG(LogTemp, Warning, TEXT("No existing save found. Created new PlayerSaveData."));
    }

    if (PlayerSaveData)
    {
        PickaxeLevel = PlayerSaveData->PickaxeLevel;
    }
    else
    {
        PickaxeLevel = 1; // 기본값
    }

    // 곡괭이 데이터와 외형 업데이트를 강제 호출하여 반영
    UpdatePickaxeData();
    SetPickaxeMesh();
}


// 곡괭이 데이터 업데이트
void UPickaxeComponent::UpdatePickaxeData()
{
    if (!PickaxeDataTable)
    {
        UE_LOG(LogTemp, Error, TEXT("PickaxeDataTable is NULL!"));
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

// 곡괭이 외형 설정
void UPickaxeComponent::SetPickaxeMesh()
{
    if (!StaticMeshComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("SetPickaxeMesh failed: StaticMeshComponent is NULL!"));
        return;
    }

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
            EffectComponent->SetAsset(nullptr);
        }
    }
}

// 곡괭이를 캐릭터 손에 부착
void UPickaxeComponent::AttachPickaxeToHand()
{
    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (OwnerCharacter && StaticMeshComponent)
    {
        USkeletalMeshComponent* MeshComp = OwnerCharacter->GetMesh();
        if (MeshComp)
        {
            StaticMeshComponent->AttachToComponent(MeshComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("hand_rSocket"));
        }
    }
}

// 곡괭이 레벨 업그레이드
void UPickaxeComponent::UpgradePickaxe()
{
    if (PlayerSaveData)
    {
        PickaxeLevel += 1;
        PlayerSaveData->PickaxeLevel = PickaxeLevel;
        PlayerSaveData->SaveGameData(); // 즉시 저장

        // 업그레이드 후 외형 업데이트
        UpdatePickaxeData();
        SetPickaxeMesh();
    }
}

float UPickaxeComponent::GetMiningSpeedBonus() const
{
    if (CurrentPickaxeData.MiningSpeedBonus)
    {
        UE_LOG(LogTemp, Warning, TEXT("보너스 존재할때 : %f"), CurrentPickaxeData.MiningSpeedBonus);
        return CurrentPickaxeData.MiningSpeedBonus;
    }

    UE_LOG(LogTemp, Warning, TEXT("보너스 존재 안할때 : %f"), MiningSpeedBonus);

    return MiningSpeedBonus;
}
