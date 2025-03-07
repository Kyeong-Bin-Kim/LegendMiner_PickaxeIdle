#include "PickaxeComponent.h"
#include "PlayerSaveData.h"
#include "GameFramework/Character.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "OreSpawner.h"
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

    // 곡괭이 외형 및 이펙트 설정
    SetPickaxeMesh();
    SetPickaxeEffect();

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

// 곡괭이 이펙트 설정
void UPickaxeComponent::SetPickaxeEffect()
{
    if (!EffectComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("SetPickaxeEffect failed: EffectComponent is NULL!"));
        return;
    }

    if (CurrentPickaxeData.Effect)
    {
        EffectComponent->SetAsset(CurrentPickaxeData.Effect);
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
            StaticMeshComponent->AttachToComponent(
                MeshComp,
                FAttachmentTransformRules::SnapToTargetNotIncludingScale,
                TEXT("hand_rSocket")
            );

            // EffectComponent를 "Aura" 소켓에 부착
            if (CurrentPickaxeData.Effect)
            {
                EffectComponent->AttachToComponent(
                    StaticMeshComponent,
                    FAttachmentTransformRules::SnapToTargetNotIncludingScale,
                    TEXT("Aura")  // 곡괭이 메쉬의 소켓 이름
                );
            }
        }
    }
}

// 곡괭이 레벨 업그레이드
void UPickaxeComponent::UpgradePickaxe()
{
    PlayerSaveData = UPlayerSaveData::LoadGameData();

    int32 CurrentPickaxeLevel = PlayerSaveData->GetPickaxeLevel();
    FName NextPickaxeRowName = FName(*FString::FromInt(CurrentPickaxeLevel + 1));

    // 다음 레벨의 곡괭이 데이터 찾기
    FPickaxeData* NextPickaxeData = PickaxeDataTable->FindRow<FPickaxeData>(NextPickaxeRowName, TEXT("Pickaxe Upgrade Lookup"));
    if (!NextPickaxeData)
    {
        return;
    }

    // 필요한 광석과 골드 정보 가져오기
    int32 RequiredOre = NextPickaxeData->UpgradeCostOre;
    int32 RequiredGold = NextPickaxeData->UpgradeCostGold;

    FName RequiredOreID = FName(*FString::FromInt(CurrentPickaxeLevel)); // 현재 곡괭이 레벨과 동일한 광석 필요

    int32 PlayerOreQuantity = PlayerSaveData->GetOreQuantity(RequiredOreID);
    int32 PlayerGold = PlayerSaveData->GetPlayerGold();

    // 비용 차감
    PlayerSaveData->RemoveOreFromInventory(RequiredOreID, RequiredOre);
    PlayerSaveData->SubtractGold(RequiredGold);

    // 곡괭이 업그레이드 실행
    PickaxeLevel = CurrentPickaxeLevel + 1;
    PlayerSaveData->SetPickaxeLevel(CurrentPickaxeLevel + 1);

    // 광석 스포너를 찾아 `RespawnAllOres()` 호출 (광석을 직접 관리하지 않음)
    AOreSpawner* OreSpawner = Cast<AOreSpawner>(UGameplayStatics::GetActorOfClass(GetWorld(), AOreSpawner::StaticClass()));
    if (OreSpawner)
    {
        OreSpawner->RespawnAllOres();
    }

    // 업그레이드 후 채굴 속도 다시 적용
    UpdatePickaxeData();

    // 곡괭이 외형 및 이펙트 변경
    SetPickaxeMesh();
    SetPickaxeEffect();
}


float UPickaxeComponent::GetMiningSpeedBonus() const
{
    if (CurrentPickaxeData.MiningSpeedBonus)
    {
        return CurrentPickaxeData.MiningSpeedBonus;
    }

    return 0.0f;
}

FPickaxeData UPickaxeComponent::GetPickaxeUpgradeData(FName PickaxeRowName)
{
    if (!PickaxeDataTable)
    {
        UE_LOG(LogTemp, Warning, TEXT("PickaxeComponent: PickaxeDataTable is NULL!"));
        return FPickaxeData(); // 기본 빈 값 반환
    }

    const FPickaxeData* PickaxeData = PickaxeDataTable->FindRow<FPickaxeData>(PickaxeRowName, TEXT("Pickaxe Upgrade Lookup"));

    if (!PickaxeData)
    {
        UE_LOG(LogTemp, Warning, TEXT("PickaxeComponent: No upgrade data found for Pickaxe Level %s"), *PickaxeRowName.ToString());
        return FPickaxeData(); // 기본 빈 값 반환
    }

    return *PickaxeData; // 값 복사하여 반환
}
