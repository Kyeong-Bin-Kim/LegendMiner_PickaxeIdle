#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ClickMarkerActor.generated.h"

class UDecalComponent;

/**
 * 클릭 지점에 스프레이 형태로 표시되는 마커 액터
 */
UCLASS()
class LEGENDMINER_PICKIDLE_API AClickMarkerActor : public AActor
{
    GENERATED_BODY()

public:
    AClickMarkerActor();

protected:
    virtual void BeginPlay() override;

    // 머티리얼 적용
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Marker")
    UDecalComponent* MarkerDecal;

public:
    // 블루프린트에서 설정할 마커 머티리얼
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Marker")
    UMaterialInterface* MarkerMaterial;

    // 디칼 크기 (X: 높이, YZ: 투영 넓이)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Marker")
    FVector DecalSize = FVector(64.f, 128.f, 96.f);

    // 바닥을 향한 회전 각도 (기본 -90 Pitch)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Marker")
    FRotator MarkerRotation = FRotator(-90.f, 50.f, 0.f);

    // 유지 시간 (초) 후 자동 제거
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Marker")
    float LifeTime = 1.0f;
};
