#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PickaxeData.h"
#include "NiagaraComponent.h"
#include "PickaxeComponent.generated.h"

class UPlayerSaveData;
class UStaticMeshComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LEGENDMINER_PICKIDLE_API UPickaxeComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UPickaxeComponent();

protected:
    virtual void BeginPlay() override;

public:
    // 곡괭이 레벨을 저장된 데이터에서 가져와 업데이트
    void LoadPickaxeLevelFromSave();

    // 곡괭이 데이터 업데이트
    UFUNCTION()
    void UpdatePickaxeData();

    // 곡괭이 외형 설정
    UFUNCTION()
    void SetPickaxeMesh();

    // 곡괭이를 캐릭터 손에 부착
    UFUNCTION()
    void AttachPickaxeToHand();

    // 곡괭이 레벨 업그레이드
    UFUNCTION(BlueprintCallable, Category = "Pickaxe")
    void UpgradePickaxe();

    float GetMiningSpeedBonus() const;

    // 곡괭이 레벨 (저장된 데이터에서 가져옴)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickaxe")
    int32 PickaxeLevel;

    // 저장된 데이터 참조
    UPROPERTY()
    UPlayerSaveData* PlayerSaveData;

    // 곡괭이 데이터 테이블 참조
    UPROPERTY(EditAnywhere, Category = "Pickaxe")
    TObjectPtr<UDataTable> PickaxeDataTable;

    // 곡괭이 메쉬 컴포넌트
    UPROPERTY()
    UStaticMeshComponent* StaticMeshComponent;

    // 곡괭이 이펙트 컴포넌트
    UPROPERTY()
    UNiagaraComponent* EffectComponent;

    // 현재 곡괭이 데이터
    FPickaxeData CurrentPickaxeData;

private:
    UPROPERTY(EditAnywhere, Category = "Pickaxe")
    float MiningSpeedBonus;
};
