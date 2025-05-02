#include "ClickMarkerActor.h"
#include "Components/DecalComponent.h"

AClickMarkerActor::AClickMarkerActor()
{
    PrimaryActorTick.bCanEverTick = false;

    MarkerDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("MarkerDecal"));
    RootComponent = MarkerDecal;
}

void AClickMarkerActor::BeginPlay()
{
    Super::BeginPlay();

    // 머티리얼 적용 (설정 안 됐으면 로그만 출력)
    if (MarkerMaterial)
    {
        MarkerDecal->SetDecalMaterial(MarkerMaterial);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("MarkerMaterial이 설정되지 않았습니다."));
    }

    // 디칼 사이즈 및 회전 설정
    MarkerDecal->DecalSize = DecalSize;
    MarkerDecal->SetRelativeRotation(MarkerRotation);

    // 자동 제거
    SetLifeSpan(LifeTime);
}
